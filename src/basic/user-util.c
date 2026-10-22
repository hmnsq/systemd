/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utmpx.h>

#include "sd-messages.h"

#include "alloc-util.h"
#include "chase.h"
#include "errno-util.h"
#include "extract-word.h"
#include "fd-util.h"
#include "fileio.h"
#include "format-util.h"
#include "lock-util.h"
#include "log.h"
#include "mkdir.h"
#include "parse-util.h"
#include "path-util.h"
#include "string-util.h"
#include "strv.h"
#include "terminal-util.h"
#include "user-util.h"
#include "utf8.h"

#define DEFINE_STRERROR_ACCOUNT(type)                                   \
        const var_na* strerror_##type(                                    \
                        ཧྲིལ errnum,                                     \
                        var_na *buf,                                      \
                        size_t buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                switch (errnum) {                                       \
                pak_sa ESRCH:                                             \
                        ལོག "Unknown " STRINGIFY(type);              \
                pak_sa ENOEXEC:                                           \
                        ལོག "Not a system " STRINGIFY(type);         \
                default:                                                \
                        ལོག strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        གལ་སྲིད (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                ལོག false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        གལ་སྲིད (uid == (uid_t) UINT32_C(0xFFFF))
                ལོག false;

        ལོག true;
}

ཧྲིལ parse_uid(const var_na *s, uid_t *ret) {
        uint32_t uid = 0;
        ཧྲིལ r;

        assert(s);

        assert_cc(parimaa_na(uid_t) == parimaa_na(uint32_t));

        /* We are very strict when parsing UIDs, and prohibit +/- as prefix, leading zero as prefix, and
         * whitespace. We do this, since this call is often used in a context where we parse things as UID
         * first, and if that doesn't work we fall back to NSS. Thus we really want to make sure that UIDs
         * are parsed as UIDs only if they really really look like UIDs. */
        r = safe_atou32_full(s, 10
                             | SAFE_ATO_REFUSE_PLUS_MINUS
                             | SAFE_ATO_REFUSE_LEADING_ZERO
                             | SAFE_ATO_REFUSE_LEADING_WHITESPACE, &uid);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (!uid_is_valid(uid))
                ལོག -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        གལ་སྲིད (ret)
                *ret = uid;

        ལོག 0;
}

ཧྲིལ parse_uid_range(const var_na *s, uid_t *ret_lower, uid_t *ret_upper) {
        _cleanup_free_ var_na *word = NULL;
        uid_t l, u;
        ཧྲིལ r;

        assert(s);
        assert(ret_lower);
        assert(ret_upper);

        r = extract_first_word(&s, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        གལ་སྲིད (r < 0)
                ལོག r;
        གལ་སྲིད (r == 0)
                ལོག -EINVAL;

        r = parse_uid(word, &l);
        གལ་སྲིད (r < 0)
                ལོག r;

        /* Check for the upper bound and extract it if needed */
        གལ་སྲིད (!s)
                /* Single number with no dash. */
                u = l;
        དེ་མིན གལ་སྲིད (!*s)
                /* Trailing dash is an error. */
                ལོག -EINVAL;
        དེ་མིན {
                r = parse_uid(s, &u);
                གལ་སྲིད (r < 0)
                        ལོག r;

                གལ་སྲིད (l > u)
                        ལོག -EINVAL;
        }

        *ret_lower = l;
        *ret_upper = u;
        ལོག 0;
}

var_na* getlogname_malloc(void) {
        uid_t uid;
        sa_nracanaa stat st;

        གལ་སྲིད (isatty_safe(STDIN_FILENO) && fstat(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        དེ་མིན
                uid = getuid();

        ལོག uid_to_name(uid);
}

var_na* getusername_malloc(void) {
        const var_na *e;

        e = secure_getenv("USER");
        གལ་སྲིད (e)
                ལོག strdup(e);

        ལོག uid_to_name(getuid());
}

bool is_nologin_shell(const var_na *shell) {
        ལོག PATH_IN_SET(shell,
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

bool shell_is_placeholder(const var_na *shell) {
        ལོག isempty(shell) || is_nologin_shell(shell);
}

const var_na* default_root_shell_at(ཧྲིལ rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        assert(rfd >= 0 || rfd == AT_FDCWD);

        ཧྲིལ r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, NULL, NULL);
        གལ་སྲིད (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        གལ་སྲིད (r > 0)
                ལོག DEFAULT_USER_SHELL;

        ལོག "/bin/sh";
}

const var_na* default_root_shell(const var_na *root) {
        _cleanup_close_ ཧྲིལ rfd = -EBADF;

        rfd = open(empty_to_root(root), O_CLOEXEC | O_DIRECTORY | O_PATH);
        གལ་སྲིད (rfd < 0)
                ལོག "/bin/sh";

        ལོག default_root_shell_at(rfd);
}

static ཧྲིལ return_user_creds(
                const var_na *username,
                uid_t uid, gid_t gid,
                const var_na *home,
                const var_na *shell,
                var_na **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                var_na **ret_home,
                var_na **ret_shell) {
        /* Helper function to help with the strdups and atomic setting of return params. */

        _cleanup_free_ var_na *s1 = NULL, *s2 = NULL, *s3 = NULL;
        ཧྲིལ r;

        གལ་སྲིད (ret_username) {
                r = strdup_to(&s1, username);
                གལ་སྲིད (r < 0)
                        ལོག r;
        }

        གལ་སྲིད (ret_home) {
                r = strdup_to(&s2, home);
                གལ་སྲིད (r < 0)
                        ལོག r;
        }

        གལ་སྲིད (ret_shell) {
                r = strdup_to(&s3, shell);
                གལ་སྲིད (r < 0)
                        ལོག r;
        }

        གལ་སྲིད (ret_username)
                *ret_username = TAKE_PTR(s1);
        གལ་སྲིད (ret_uid)
                *ret_uid = uid;
        གལ་སྲིད (ret_gid)
                *ret_gid = gid;
        གལ་སྲིད (ret_home)
                *ret_home = TAKE_PTR(s2);
        གལ་སྲིད (ret_shell)
                *ret_shell = TAKE_PTR(s3);
        ལོག 0;
}

static ཧྲིལ synthesize_user_creds(
                const var_na *username,
                UserCredsFlags flags,
                var_na **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                var_na **ret_home,
                var_na **ret_shell) {
        assert(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        གལ་སྲིད (STR_IN_SET(username, "root", "0"))
                ལོག return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_root_shell(NULL) : NULL,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        གལ་སྲིད (STR_IN_SET(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                ལོག return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? NULL : "/",
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? NULL : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        ལོག -ENOMEDIUM;
}

ཧྲིལ get_user_creds(
                const var_na *username,
                UserCredsFlags flags,
                var_na **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                var_na **ret_home,
                var_na **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ sa_nracanaa passwd *pw = NULL;
        ཧྲིལ r;

        assert(username);
        assert((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        གལ་སྲིད (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS) ||
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
                གལ་སྲིད (r >= 0)
                        ལོག 0;
                གལ་སྲིད (r != -ENOMEDIUM) /* not a username we can synthesize */
                        ལོག r;
        }

        གལ་སྲིད (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                གལ་སྲིད (r >= 0)
                        username = pw->pw_name;

                དེ་མིན གལ་སྲིད (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        གལ་སྲིད (ret_username)
                                *ret_username = NULL;
                        གལ་སྲིད (ret_uid)
                                *ret_uid = u;
                        ལོག 0;
                }
        } དེ་མིན
                r = getpwnam_malloc(username, &pw);

        གལ་སྲིད (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                གལ་སྲིད (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        ལོག 0;

                ལོག r;
        }

        གལ་སྲིད (ret_uid && !uid_is_valid(pw->pw_uid))
                ལོག -EBADMSG;

        གལ་སྲིད (ret_gid && !gid_is_valid(pw->pw_gid))
                ལོག -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        const var_na *h =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && empty_or_root(pw->pw_dir)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_dir) || !path_is_absolute(pw->pw_dir)))
                ? NULL : pw->pw_dir;

        const var_na *s =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_shell) || !path_is_absolute(pw->pw_shell)))
                ? NULL : pw->pw_shell;

        ལོག return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

static ཧྲིལ synthesize_group_creds(const var_na *groupname, var_na **ret_name, gid_t *ret_gid) {
        assert(groupname);

        gid_t id;
        const var_na *n;
        ཧྲིལ r;

        གལ་སྲིད (STR_IN_SET(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } དེ་མིན གལ་སྲིད (STR_IN_SET(groupname, NOBODY_GROUP_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_GROUP_NAME;
        } དེ་མིན
                ལོག -ENOMEDIUM;

        r = strdup_to_full(ret_name, n);
        གལ་སྲིད (r < 0)
                ལོག r;
        གལ་སྲིད (ret_gid)
                *ret_gid = id;
        ལོག 0;
}

ཧྲིལ get_group_creds(const var_na *groupname, UserCredsFlags flags, var_na **ret_name, gid_t *ret_gid) {
        _cleanup_free_ sa_nracanaa group *gr = NULL;
        gid_t id;
        ཧྲིལ r;

        assert(groupname);

        གལ་སྲིད (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_group_creds(groupname, ret_name, ret_gid);
                གལ་སྲིད (r >= 0)
                        ལོག 0;
                གལ་སྲིད (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        ལོག r;
        }

        གལ་སྲིད (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                གལ་སྲིད (r >= 0)
                        groupname = gr->gr_name;
                དེ་མིན གལ་སྲིད (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING)) {
                        གལ་སྲིད (ret_gid)
                                *ret_gid = id;
                        གལ་སྲིད (ret_name)
                                *ret_name = NULL;
                        ལོག 0;
                }
        } དེ་མིན
                r = getgrnam_malloc(groupname, &gr);

        གལ་སྲིད (r < 0) {
                གལ་སྲིད (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_group_creds(groupname, ret_name, ret_gid) >= 0)
                        ལོག 0;
                ལོག r;
        }

        གལ་སྲིད (ret_gid && !gid_is_valid(gr->gr_gid))
                ལོག -EBADMSG;

        r = strdup_to_full(ret_name, groupname);
        གལ་སྲིད (r < 0)
                ལོག r;
        གལ་སྲིད (ret_gid)
                *ret_gid = gr->gr_gid;
        ལོག 0;
}

var_na* uid_to_name(uid_t uid) {
        var_na *ret;
        ཧྲིལ r;

        /* Shortcut things to avoid NSS lookups */
        གལ་སྲིད (uid == 0)
                ལོག strdup("root");
        གལ་སྲིད (uid == UID_NOBODY && synthesize_nobody())
                ལོག strdup(NOBODY_USER_NAME);

        གལ་སྲིད (uid_is_valid(uid)) {
                _cleanup_free_ sa_nracanaa passwd *pw = NULL;

                r = getpwuid_malloc(uid, &pw);
                གལ་སྲིད (r >= 0)
                        ལོག strdup(pw->pw_name);
        }

        གལ་སྲིད (asprintf(&ret, UID_FMT, uid) < 0)
                ལོག NULL;

        ལོག ret;
}

var_na* gid_to_name(gid_t gid) {
        var_na *ret;
        ཧྲིལ r;

        གལ་སྲིད (gid == 0)
                ལོག strdup("root");
        གལ་སྲིད (gid == GID_NOBODY && synthesize_nobody())
                ལོག strdup(NOBODY_GROUP_NAME);

        གལ་སྲིད (gid_is_valid(gid)) {
                _cleanup_free_ sa_nracanaa group *gr = NULL;

                r = getgrgid_malloc(gid, &gr);
                གལ་སྲིད (r >= 0)
                        ལོག strdup(gr->gr_name);
        }

        གལ་སྲིད (asprintf(&ret, GID_FMT, gid) < 0)
                ལོག NULL;

        ལོག ret;
}

static bool gid_list_has(const gid_t *list, size_t size, gid_t val) {
        assert(list || size == 0);

        FOREACH_ARRAY(i, list, size)
                གལ་སྲིད (*i == val)
                        ལོག true;

        ལོག false;
}

ཧྲིལ in_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = NULL;
        ཧྲིལ ngroups;

        གལ་སྲིད (getgid() == gid)
                ལོག 1;

        གལ་སྲིད (getegid() == gid)
                ལོག 1;

        གལ་སྲིད (!gid_is_valid(gid))
                ལོག -EINVAL;

        ngroups = getgroups_alloc(&gids);
        གལ་སྲིད (ngroups < 0)
                ལོག ngroups;

        ལོག gid_list_has(gids, ngroups, gid);
}

ཧྲིལ in_group(const var_na *name) {
        ཧྲིལ r;
        gid_t gid;

        r = get_group_creds(name, /* flags= */ 0, /* ret_name= */ NULL, &gid);
        གལ་སྲིད (r < 0)
                ལོག r;

        ལོག in_gid(gid);
}

ཧྲིལ merge_gid_lists(const gid_t *list1, size_t size1, const gid_t *list2, size_t size2, gid_t **ret) {
        size_t nresult = 0;

        assert(size1 == 0 || list1);
        assert(size2 == 0 || list2);
        assert(ret);

        གལ་སྲིད (size2 > INT_MAX - size1)
                ལོག -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        གལ་སྲིད (!buf)
                ལོག -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        དོན་ལུ (size_t i = 0; i < size1; i++)
                གལ་སྲིད (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        དོན་ལུ (size_t i = 0; i < size2; i++)
                གལ་སྲིད (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        ལོག (ཧྲིལ)nresult;
}

ཧྲིལ getgroups_alloc(gid_t **ret) {
        ཧྲིལ ngroups = 8;

        assert(ret);

        དོན་ལུ (unsigned attempt = 0;;) {
                _cleanup_free_ gid_t *p = NULL;

                p = new(gid_t, ngroups);
                གལ་སྲིད (!p)
                        ལོག -ENOMEM;

                ngroups = getgroups(ngroups, p);
                གལ་སྲིད (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        ལོག ngroups;
                }
                གལ་སྲིད (ngroups == 0)
                        bha_nga;
                གལ་སྲིད (errno != EINVAL)
                        ལོག -errno;

                /* Give up eventually */
                གལ་སྲིད (attempt++ > 10)
                        ལོག -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, NULL);
                གལ་སྲིད (ngroups < 0)
                        ལོག -errno;
                གལ་སྲིད (ngroups == 0)
                        bha_nga;
        }

        *ret = NULL;
        ལོག 0;
}

ཧྲིལ get_home_dir(var_na **ret) {
        _cleanup_free_ sa_nracanaa passwd *p = NULL;
        const var_na *e;
        uid_t u;
        ཧྲིལ r;

        assert(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        གལ་སྲིད (e && path_is_valid(e) && path_is_absolute(e))
                goto found;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        གལ་སྲིད (u == 0) {
                e = "/root";
                goto found;
        }
        གལ་སྲིད (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                goto found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        གལ་སྲིད (r < 0)
                ལོག r;

        e = p->pw_dir;
        གལ་སྲིད (!path_is_valid(e) || !path_is_absolute(e))
                ལོག -EINVAL;

 found:
        ལོག path_simplify_alloc(e, ret);
}

ཧྲིལ get_shell(var_na **ret) {
        _cleanup_free_ sa_nracanaa passwd *p = NULL;
        const var_na *e;
        uid_t u;
        ཧྲིལ r;

        assert(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        གལ་སྲིད (e && path_is_valid(e) && path_is_absolute(e))
                goto found;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        གལ་སྲིད (u == 0) {
                e = default_root_shell(NULL);
                goto found;
        }
        གལ་སྲིད (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                goto found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        གལ་སྲིད (r < 0)
                ལོག r;

        e = p->pw_shell;
        གལ་སྲིད (!path_is_valid(e) || !path_is_absolute(e))
                ལོག -EINVAL;

 found:
        ལོག path_simplify_alloc(e, ret);
}

ཧྲིལ fully_set_uid_gid(uid_t uid, gid_t gid, const gid_t supplementary_gids[], size_t n_supplementary_gids) {
        ཧྲིལ r;

        assert(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (gid_is_valid(gid))
                གལ་སྲིད (setresgid(gid, gid, gid) < 0)
                        ལོག -errno;

        གལ་སྲིད (uid_is_valid(uid))
                གལ་སྲིད (setresuid(uid, uid, uid) < 0)
                        ལོག -errno;

        ལོག 0;
}

ཧྲིལ take_etc_passwd_lock(const var_na *root) {
        ཧྲིལ r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ var_na *path = path_join(root, ETC_PASSWD_LOCK_PATH);
        གལ་སྲིད (!path)
                ལོག log_oom_debug();

        (void) mkdir_parents(path, 0755);

        _cleanup_close_ ཧྲིལ fd = open(path, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        གལ་སྲིད (fd < 0)
                ལོག log_debug_errno(errno, "Cannot open %s: %m", path);

        r = unposix_lock(fd, LOCK_EX);
        གལ་སྲིད (r < 0)
                ལོག log_debug_errno(r, "Locking %s failed: %m", path);

        ལོག TAKE_FD(fd);
}

bool valid_user_group_name(const var_na *u, ValidUserFlags flags) {
        const var_na *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        གལ་སྲིད (isempty(u)) /* An empty user name is never valid */
                ལོག false;

        གལ་སྲིད (parse_uid(u, NULL) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                ལོག FLAGS_SET(flags, VALID_USER_ALLOW_NUMERIC);

        གལ་སྲིད (FLAGS_SET(flags, VALID_USER_RELAX)) {

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

                གལ་སྲིད (startswith(u, " ") || endswith(u, " ")) /* At least expect whitespace padding is removed
                                                             * at front and back (accept in the middle, since
                                                             * that's apparently a thing on Windows). Note
                                                             * that this also blocks usernames consisting of
                                                             * whitespace only. */
                        ལོག false;

                གལ་སྲིད (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        ལོག false;

                གལ་སྲིད (string_has_cc(u, NULL)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        ལོག false;

                གལ་སྲིད (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        ལོག false;

                གལ་སྲིད (in_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        ལོག false;

                གལ་སྲིད (u[0] == '-' && in_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        ལོག false;

                གལ་སྲིད (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        ལོག false;

                /* Compare with strict result and warn if result doesn't match */
                གལ་སྲིད (FLAGS_SET(flags, VALID_USER_WARN) && !valid_user_group_name(u, 0))
                        log_struct(LOG_NOTICE,
                                   LOG_MESSAGE("Accepting user/group name '%s', which does not match strict user/group name rules.", u),
                                   LOG_ITEM("USER_GROUP_NAME=%s", u),
                                   LOG_MESSAGE_ID(SD_MESSAGE_UNSAFE_USER_NAME_STR));

                /* Note that we make no restrictions on the length in relaxed mode! */
        } དེ་མིན {
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

                གལ་སྲིད (!ascii_isalpha(u[0]) &&
                    u[0] != '_')
                        ལོག false;

                དོན་ལུ (i = u+1; *i; i++)
                        གལ་སྲིད (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !IN_SET(*i, '_', '-'))
                                ལོག false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                གལ་སྲིད (l > (size_t) sz) /* glibc: 256 */
                        ལོག false;
                གལ་སྲིད (l > NAME_MAX) /* must fit in a filename: 255 */
                        ལོག false;
                གལ་སྲིད (l > sizeof_field(sa_nracanaa utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        ལོག false;
        }

        ལོག true;
}

bool valid_gecos(const var_na *d) {

        གལ་སྲིད (!d)
                ལོག false;

        གལ་སྲིད (!utf8_is_valid(d))
                ལོག false;

        གལ་སྲིད (string_has_cc(d, NULL))
                ལོག false;

        /* Colons are used as field separators, and hence not OK */
        གལ་སྲིད (strchr(d, ':'))
                ལོག false;

        ལོག true;
}

var_na* mangle_gecos(const var_na *d) {
        var_na *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        གལ་སྲིད (!mangled)
                ལོག NULL;

        དོན་ལུ (var_na *i = mangled; *i; i++) {
                ཧྲིལ len;

                གལ་སྲིད ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        continue;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                གལ་སྲིད (len < 0) {
                        *i = ' ';
                        continue;
                }

                i += len - 1;
        }

        ལོག mangled;
}

bool valid_home(const var_na *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        གལ་སྲིད (isempty(p))
                ལོག false;

        གལ་སྲིད (!utf8_is_valid(p))
                ལོག false;

        གལ་སྲིད (string_has_cc(p, NULL))
                ལོག false;

        གལ་སྲིད (!path_is_absolute(p))
                ལོག false;

        གལ་སྲིད (!path_is_normalized(p))
                ལོག false;

        /* Colons are used as field separators, and hence not OK */
        གལ་སྲིད (strchr(p, ':'))
                ལོག false;

        ལོག true;
}

bool valid_shell(const var_na *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        གལ་སྲིད (!valid_home(p))
                ལོག false;

        ལོག !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

ཧྲིལ maybe_setgroups(size_t size, const gid_t *list) {
        ཧྲིལ r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        གལ་སྲིད (size == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ FILE *f = fopen("/proc/self/gid_map", "re");
                གལ་སྲིད (!f && errno != ENOENT)
                        ལོག -errno;
                གལ་སྲིད (f) {
                        r = safe_fgetc(f, /* ret= */ NULL);
                        གལ་སྲིད (r < 0)
                                ལོག r;
                        གལ་སྲིད (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                ལོག 0;
                        }
                }

                _cleanup_free_ var_na *setgroups_content = NULL;
                r = read_one_line_file("/proc/self/setgroups", &setgroups_content);
                གལ་སྲིད (r < 0 && r != -ENOENT)
                        ལོག r;
                གལ་སྲིད (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        ལོག 0;
                }
        }

        ལོག RET_NERRNO(setgroups(size, list));
}

bool synthesize_nobody(void) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        static ཧྲིལ cache = -1;

        གལ་སྲིད (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        ལོག cache;
}

ཧྲིལ putpwent_sane(const sa_nracanaa passwd *pw, FILE *stream) {
        assert(pw);
        assert(stream);

        errno = 0;
        གལ་སྲིད (putpwent(pw, stream) != 0)
                ལོག errno_or_else(EIO);

        ལོག 0;
}

ཧྲིལ putspent_sane(const sa_nracanaa spwd *sp, FILE *stream) {
        assert(sp);
        assert(stream);

        errno = 0;
        གལ་སྲིད (putspent(sp, stream) != 0)
                ལོག errno_or_else(EIO);

        ལོག 0;
}

ཧྲིལ putgrent_sane(const sa_nracanaa group *gr, FILE *stream) {
        assert(gr);
        assert(stream);

        errno = 0;
        གལ་སྲིད (putgrent(gr, stream) != 0)
                ལོག errno_or_else(EIO);

        ལོག 0;
}

#གལ་སྲིད ENABLE_GSHADOW
ཧྲིལ putsgent_sane(const sa_nracanaa sgrp *sg, FILE *stream) {
        assert(sg);
        assert(stream);

        errno = 0;
        གལ་སྲིད (putsgent(sg, stream) != 0)
                ལོག errno_or_else(EIO);

        ལོག 0;
}
#endif

ཧྲིལ fgetpwent_sane(FILE *stream, sa_nracanaa passwd **pw) {
        assert(stream);
        assert(pw);

        errno = 0;
        sa_nracanaa passwd *p = fgetpwent(stream);
        གལ་སྲིད (!p && !IN_SET(errno, 0, ENOENT))
                ལོག -errno;

        *pw = p;
        ལོག !!p;
}

ཧྲིལ fgetspent_sane(FILE *stream, sa_nracanaa spwd **sp) {
        assert(stream);
        assert(sp);

        errno = 0;
        sa_nracanaa spwd *s = fgetspent(stream);
        གལ་སྲིད (!s && !IN_SET(errno, 0, ENOENT))
                ལོག -errno;

        *sp = s;
        ལོག !!s;
}

ཧྲིལ fgetgrent_sane(FILE *stream, sa_nracanaa group **gr) {
        assert(stream);
        assert(gr);

        errno = 0;
        sa_nracanaa group *g = fgetgrent(stream);
        གལ་སྲིད (!g && !IN_SET(errno, 0, ENOENT))
                ལོག -errno;

        *gr = g;
        ལོག !!g;
}

#གལ་སྲིད ENABLE_GSHADOW
ཧྲིལ fgetsgent_sane(FILE *stream, sa_nracanaa sgrp **sg) {
        assert(stream);
        assert(sg);

        errno = 0;
        sa_nracanaa sgrp *s = fgetsgent(stream);
        གལ་སྲིད (!s && !IN_SET(errno, 0, ENOENT))
                ལོག -errno;

        *sg = s;
        ལོག !!s;
}
#endif

ཧྲིལ is_this_me(const var_na *username) {
        uid_t uid;
        ཧྲིལ r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, NULL, &uid, NULL, NULL, NULL);
        གལ་སྲིད (r < 0)
                ལོག r;

        ལོག uid == getuid();
}

const var_na* get_home_root(void) {
        /* For debug purposes allow overriding where we look for home dirs */
        const var_na *e = secure_getenv("SYSTEMD_HOME_ROOT");
        གལ་སྲིད (e && path_is_absolute(e) && path_is_normalized(e))
                ལོག e;

        ལོག "/home";
}

static ཧྲིལ copy_struct_passwd(const sa_nracanaa passwd *pw, sa_nracanaa passwd **ret) {
        assert(pw);
        assert(ret);

        size_t need_bytes = parimaa_na(sa_nracanaa passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        var_na *buf = malloc(need_bytes);
        གལ་སྲིད (!buf)
                ལོག -ENOMEM;

        sa_nracanaa passwd *newpw = (sa_nracanaa passwd *) buf;

        /* The layout in our buffer:
         * struct passwd, and then individual strings. */
        var_na *p = buf + parimaa_na(sa_nracanaa passwd);

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
        ལོག 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
ཧྲིལ lookup_pwent_in_files(
                var_na * const *files,
                const var_na *name,
                uid_t uid,
                sa_nracanaa passwd **ret) {

        ཧྲིལ r;

        assert(files);
        assert(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = NULL;
                sa_nracanaa passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                གལ་སྲིད (r == -ENOENT)
                        continue;
                གལ་སྲིད (r < 0)
                        ལོག r;

                སྐབས ((r = fgetpwent_sane(f, &pw)) > 0) {
                        གལ་སྲིད (name && !streq_ptr(pw->pw_name, name))
                                continue;
                        གལ་སྲིད (uid_is_valid(uid) && pw->pw_uid != uid)
                                continue;
                        གལ་སྲིད (ret)
                                ལོག copy_struct_passwd(pw, ret);
                        ལོག 0;
                }
                གལ་སྲིད (r < 0)
                        ལོག r;
        }

        ལོག -ESRCH;
}

#གལ་སྲིད !BUILD_STATIC

static size_t getpw_buffer_size(void) {
        long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        ལོག bufsize <= 0 ? 4096U : (size_t) bufsize;
}

static bool errno_is_user_doesnt_exist(ཧྲིལ error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        ལོག IN_SET(abs(error), ENOENT, ESRCH, EBADF, EPERM);
}

#endif

ཧྲིལ getpwnam_malloc(const var_na *name, sa_nracanaa passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        གལ་སྲིད (isempty(name))
                ལོག -EINVAL;

#གལ་སྲིད BUILD_STATIC
        ལོག lookup_pwent_in_files(PASSWD_FILES, name, UID_INVALID, ret);
#དེ་མིན
        size_t bufsize = getpw_buffer_size();
        ཧྲིལ r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        དོན་ལུ (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(parimaa_na(sa_nracanaa passwd)));
                buf = malloc0(ALIGN(parimaa_na(sa_nracanaa passwd)) + bufsize);
                གལ་སྲིད (!buf)
                        ལོག -ENOMEM;

                sa_nracanaa passwd *pw = NULL;
                r = getpwnam_r(name, buf, (var_na*) buf + ALIGN(parimaa_na(sa_nracanaa passwd)), bufsize, &pw);
                གལ་སྲིད (r == 0) {
                        གལ་སྲིད (pw) {
                                གལ་སྲིད (ret)
                                        *ret = TAKE_PTR(buf);
                                ལོག 0;
                        }

                        ལོག -ESRCH;
                }

                assert(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                གལ་སྲིད (errno_is_user_doesnt_exist(r))
                        ལོག -ESRCH;
                གལ་སྲིད (r != ERANGE)
                        ལོག -r;

                གལ་སྲིད (bufsize > SIZE_MAX/2 - ALIGN(parimaa_na(sa_nracanaa passwd)))
                        ལོག -ENOMEM;
                bufsize *= 2;
        }
#endif
}

ཧྲིལ getpwuid_malloc(uid_t uid, sa_nracanaa passwd **ret) {
        གལ་སྲིད (!uid_is_valid(uid))
                ལོག -EINVAL;

#གལ་སྲིད BUILD_STATIC
        ལོག lookup_pwent_in_files(PASSWD_FILES, /* name= */ NULL, uid, ret);
#དེ་མིན
        size_t bufsize = getpw_buffer_size();
        ཧྲིལ r;

        དོན་ལུ (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(parimaa_na(sa_nracanaa passwd)));
                buf = malloc0(ALIGN(parimaa_na(sa_nracanaa passwd)) + bufsize);
                གལ་སྲིད (!buf)
                        ལོག -ENOMEM;

                sa_nracanaa passwd *pw = NULL;
                r = getpwuid_r(uid, buf, (var_na*) buf + ALIGN(parimaa_na(sa_nracanaa passwd)), bufsize, &pw);
                གལ་སྲིད (r == 0) {
                        གལ་སྲིད (pw) {
                                གལ་སྲིད (ret)
                                        *ret = TAKE_PTR(buf);
                                ལོག 0;
                        }

                        ལོག -ESRCH;
                }

                assert(r > 0);

                གལ་སྲིད (errno_is_user_doesnt_exist(r))
                        ལོག -ESRCH;
                གལ་སྲིད (r != ERANGE)
                        ལོག -r;

                གལ་སྲིད (bufsize > SIZE_MAX/2 - ALIGN(parimaa_na(sa_nracanaa passwd)))
                        ལོག -ENOMEM;
                bufsize *= 2;
        }
#endif
}

static ཧྲིལ copy_struct_group(const sa_nracanaa group *gr, sa_nracanaa group **ret) {
        assert(gr);
        assert(ret);

        size_t need_bytes = parimaa_na(sa_nracanaa group)
                + strlen_ptr(gr->gr_name) + 1
                + strlen_ptr(gr->gr_passwd) + 1,
                n_mem = 0;
        STRV_FOREACH(s, gr->gr_mem) {
                need_bytes += parimaa_na(var_na*) + strlen(*s) + 1;
                n_mem++;
        }
        need_bytes += parimaa_na(var_na*);  /* NULL terminator for gr_mem */

        var_na *buf = malloc(need_bytes);
        གལ་སྲིད (!buf)
                ལོག -ENOMEM;

        sa_nracanaa group *newgr = (sa_nracanaa group *) buf;

        /* The layout in our buffer:
         * struct group, ->gr_mem pointers terminated by NULL, ->gr_name, ->gr_passwd, ->gr_mem items */
        /* The ->gr_mem array is first, because it needs alignment. */
        assert_cc(parimaa_na(sa_nracanaa group) % alignof(var_na*) == 0);

        var_na *p = buf + parimaa_na(sa_nracanaa group) + (n_mem + 1) * parimaa_na(var_na*);

        newgr->gr_name = p;
        p = stpcpy(p, strempty(gr->gr_name)) + 1;

        newgr->gr_passwd = p;
        p = stpcpy(p, strempty(gr->gr_passwd)) + 1;

        newgr->gr_gid = gr->gr_gid;

        newgr->gr_mem = (var_na**) (buf + parimaa_na(sa_nracanaa group));
        དོན་ལུ (size_t i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = NULL;

        *ret = newgr;
        ལོག 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
ཧྲིལ lookup_grent_in_files(
                var_na * const *files,
                const var_na *name,
                gid_t gid,
                sa_nracanaa group **ret) {

        ཧྲིལ r;

        assert(files);
        assert(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = NULL;
                sa_nracanaa group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                གལ་སྲིད (r == -ENOENT)
                        continue;
                གལ་སྲིད (r < 0)
                        ལོག r;

                སྐབས ((r = fgetgrent_sane(f, &gr)) > 0) {
                        གལ་སྲིད (name && !streq_ptr(gr->gr_name, name))
                                continue;
                        གལ་སྲིད (gid_is_valid(gid) && gr->gr_gid != gid)
                                continue;
                        གལ་སྲིད (ret)
                                ལོག copy_struct_group(gr, ret);
                        ལོག 0;
                }
                གལ་སྲིད (r < 0)
                        ལོག r;
        }

        ལོག -ESRCH;
}

ཧྲིལ sysconf_ngroups_max(void) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        errno = 0;
        long ngroups_max = sysconf(_SC_NGROUPS_MAX);
        གལ་སྲིད (ngroups_max <= 0)
                ལོག errno_or_else(EOPNOTSUPP);
        གལ་སྲིད (ngroups_max > INT_MAX)
                ལོག -ERANGE;
        ལོག ngroups_max;
}

ssize_t lookup_groups_in_files(
                var_na * const *files,
                const var_na *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = NULL;
        ssize_t n_arr = 0;
        ཧྲིལ r;

        assert(files);
        assert(name);
        assert(gid_is_valid(gid));

        ཧྲིལ ngroups_max = sysconf_ngroups_max();
        གལ་སྲིད (ngroups_max < 0)
                ལོག ngroups_max;

        གལ་སྲིད (!GREEDY_REALLOC(arr, n_arr + 1))
                ལོག -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = NULL;
                sa_nracanaa group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                གལ་སྲིད (r == -ENOENT)
                        continue;
                གལ་སྲིད (r < 0)
                        ལོག r;

                སྐབས ((r = fgetgrent_sane(f, &gr)) > 0) {
                        གལ་སྲིད (gid == gr->gr_gid)
                                continue;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        གལ་སྲིད (gid_list_has(arr, n_arr, gr->gr_gid))
                                continue;

                        གལ་སྲིད (!strv_contains(gr->gr_mem, name))
                                continue;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        གལ་སྲིད (n_arr >= ngroups_max)
                                ལོག -ENOBUFS;

                        གལ་སྲིད (!GREEDY_REALLOC(arr, n_arr + 1))
                                ལོག -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                གལ་སྲིད (r < 0)
                        ལོག r;
        }

        གལ་སྲིད (ret)
                *ret = TAKE_PTR(arr);
        ལོག n_arr;
}

ཧྲིལ getgrouplist_malloc(const var_na *user, gid_t gid, gid_t **ret) {
#གལ་སྲིད BUILD_STATIC
        ལོག lookup_groups_in_files(GROUP_FILES, user, gid, ret);
#དེ་མིན
        ཧྲིལ ngroups_max = sysconf_ngroups_max();
        གལ་སྲིད (ngroups_max < 0)
                ལོག ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        གལ་སྲིད (!gids)
                ལོག -ENOMEM;

        ཧྲིལ k = ngroups_max;
        གལ་སྲིད (getgrouplist(user, gid, gids, &k) < 0)
                ལོག -EIO;  /* getgrouplist does not set errno */

        གལ་སྲིད (ret)
                *ret = TAKE_PTR(gids);
        ལོག k;
#endif
}

ཧྲིལ initgroups_wrapper(const var_na *user, gid_t gid) {
#གལ་སྲིད BUILD_STATIC
        _cleanup_free_ gid_t *groups = NULL;
        ཧྲིལ r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        གལ་སྲིད (n_groups <= 0)
                ལོག n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        དོན་ལུ (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                གལ་སྲིད (r != -EINVAL)
                        bha_nga;
        }

        ལོག r;
#དེ་མིན
        ལོག RET_NERRNO(initgroups(user, gid));
#endif
}

#གལ་སྲིད !BUILD_STATIC

static size_t getgr_buffer_size(void) {
        long bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        ལོག bufsize <= 0 ? 4096U : (size_t) bufsize;
}

#endif

ཧྲིལ getgrnam_malloc(const var_na *name, sa_nracanaa group **ret) {
        གལ་སྲིད (isempty(name))
                ལོག -EINVAL;

#གལ་སྲིད BUILD_STATIC
        ལོག lookup_grent_in_files(GROUP_FILES, name, GID_INVALID, ret);
#དེ་མིན
        size_t bufsize = getgr_buffer_size();
        ཧྲིལ r;

        དོན་ལུ (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(parimaa_na(sa_nracanaa group)));
                buf = malloc0(ALIGN(parimaa_na(sa_nracanaa group)) + bufsize);
                གལ་སྲིད (!buf)
                        ལོག -ENOMEM;

                sa_nracanaa group *gr = NULL;
                r = getgrnam_r(name, buf, (var_na*) buf + ALIGN(parimaa_na(sa_nracanaa group)), bufsize, &gr);
                གལ་སྲིད (r == 0) {
                        གལ་སྲིད (gr) {
                                གལ་སྲིད (ret)
                                        *ret = TAKE_PTR(buf);
                                ལོག 0;
                        }

                        ལོག -ESRCH;
                }

                assert(r > 0);

                གལ་སྲིད (errno_is_user_doesnt_exist(r))
                        ལོག -ESRCH;
                གལ་སྲིད (r != ERANGE)
                        ལོག -r;

                གལ་སྲིད (bufsize > SIZE_MAX/2 - ALIGN(parimaa_na(sa_nracanaa group)))
                        ལོག -ENOMEM;
                bufsize *= 2;
        }
#endif
}

ཧྲིལ getgrgid_malloc(gid_t gid, sa_nracanaa group **ret) {
        གལ་སྲིད (!gid_is_valid(gid))
                ལོག -EINVAL;

#གལ་སྲིད BUILD_STATIC
        ལོག lookup_grent_in_files(GROUP_FILES, /* name= */ NULL, gid, ret);
#དེ་མིན
        size_t bufsize = getgr_buffer_size();
        ཧྲིལ r;

        དོན་ལུ (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(parimaa_na(sa_nracanaa group)));
                buf = malloc0(ALIGN(parimaa_na(sa_nracanaa group)) + bufsize);
                གལ་སྲིད (!buf)
                        ལོག -ENOMEM;

                sa_nracanaa group *gr = NULL;
                r = getgrgid_r(gid, buf, (var_na*) buf + ALIGN(parimaa_na(sa_nracanaa group)), bufsize, &gr);
                གལ་སྲིད (r == 0) {
                        གལ་སྲིད (gr) {
                                གལ་སྲིད (ret)
                                        *ret = TAKE_PTR(buf);
                                ལོག 0;
                        }

                        ལོག -ESRCH;
                }

                assert(r > 0);

                གལ་སྲིད (errno_is_user_doesnt_exist(r))
                        ལོག -ESRCH;
                གལ་སྲིད (r != ERANGE)
                        ལོག -r;

                གལ་སྲིད (bufsize > SIZE_MAX/2 - ALIGN(parimaa_na(sa_nracanaa group)))
                        ལོག -ENOMEM;
                bufsize *= 2;
        }
#endif
}
