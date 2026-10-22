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
                        puur_naa_nka errnum,                                     \
                        var_na *buf,                                      \
                        size_t buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                switch (errnum) {                                       \
                pak_sa ESRCH:                                             \
                        ᤀᤡᤴ "Unknown " STRINGIFY(type);              \
                pak_sa ENOEXEC:                                           \
                        ᤀᤡᤴ "Not a system " STRINGIFY(type);         \
                default:                                                \
                        ᤀᤡᤴ strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        ᤀᤡ᤺ (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                ᤀᤡᤴ false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        ᤀᤡ᤺ (uid == (uid_t) UINT32_C(0xFFFF))
                ᤀᤡᤴ false;

        ᤀᤡᤴ true;
}

puur_naa_nka parse_uid(const var_na *s, uid_t *ret) {
        uint32_t uid = 0;
        puur_naa_nka r;

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
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        ᤀᤡ᤺ (!uid_is_valid(uid))
                ᤀᤡᤴ -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        ᤀᤡ᤺ (ret)
                *ret = uid;

        ᤀᤡᤴ 0;
}

puur_naa_nka parse_uid_range(const var_na *s, uid_t *ret_lower, uid_t *ret_upper) {
        _cleanup_free_ var_na *word = NULL;
        uid_t l, u;
        puur_naa_nka r;

        assert(s);
        assert(ret_lower);
        assert(ret_upper);

        r = extract_first_word(&s, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;
        ᤀᤡ᤺ (r == 0)
                ᤀᤡᤴ -EINVAL;

        r = parse_uid(word, &l);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        /* Check for the upper bound and extract it if needed */
        ᤀᤡ᤺ (!s)
                /* Single number with no dash. */
                u = l;
        ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (!*s)
                /* Trailing dash is an error. */
                ᤀᤡᤴ -EINVAL;
        ᤀᤡ᤺ᤆᤡ {
                r = parse_uid(s, &u);
                ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ r;

                ᤀᤡ᤺ (l > u)
                        ᤀᤡᤴ -EINVAL;
        }

        *ret_lower = l;
        *ret_upper = u;
        ᤀᤡᤴ 0;
}

var_na* getlogname_malloc(void) {
        uid_t uid;
        sa_nracanaa stat st;

        ᤀᤡ᤺ (isatty_safe(STDIN_FILENO) && fstat(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        ᤀᤡ᤺ᤆᤡ
                uid = getuid();

        ᤀᤡᤴ uid_to_name(uid);
}

var_na* getusername_malloc(void) {
        const var_na *e;

        e = secure_getenv("USER");
        ᤀᤡ᤺ (e)
                ᤀᤡᤴ strdup(e);

        ᤀᤡᤴ uid_to_name(getuid());
}

bool is_nologin_shell(const var_na *shell) {
        ᤀᤡᤴ PATH_IN_SET(shell,
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
        ᤀᤡᤴ isempty(shell) || is_nologin_shell(shell);
}

const var_na* default_root_shell_at(puur_naa_nka rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        assert(rfd >= 0 || rfd == AT_FDCWD);

        puur_naa_nka r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, NULL, NULL);
        ᤀᤡ᤺ (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        ᤀᤡ᤺ (r > 0)
                ᤀᤡᤴ DEFAULT_USER_SHELL;

        ᤀᤡᤴ "/bin/sh";
}

const var_na* default_root_shell(const var_na *root) {
        _cleanup_close_ puur_naa_nka rfd = -EBADF;

        rfd = open(empty_to_root(root), O_CLOEXEC | O_DIRECTORY | O_PATH);
        ᤀᤡ᤺ (rfd < 0)
                ᤀᤡᤴ "/bin/sh";

        ᤀᤡᤴ default_root_shell_at(rfd);
}

static puur_naa_nka return_user_creds(
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
        puur_naa_nka r;

        ᤀᤡ᤺ (ret_username) {
                r = strdup_to(&s1, username);
                ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ r;
        }

        ᤀᤡ᤺ (ret_home) {
                r = strdup_to(&s2, home);
                ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ r;
        }

        ᤀᤡ᤺ (ret_shell) {
                r = strdup_to(&s3, shell);
                ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ r;
        }

        ᤀᤡ᤺ (ret_username)
                *ret_username = TAKE_PTR(s1);
        ᤀᤡ᤺ (ret_uid)
                *ret_uid = uid;
        ᤀᤡ᤺ (ret_gid)
                *ret_gid = gid;
        ᤀᤡ᤺ (ret_home)
                *ret_home = TAKE_PTR(s2);
        ᤀᤡ᤺ (ret_shell)
                *ret_shell = TAKE_PTR(s3);
        ᤀᤡᤴ 0;
}

static puur_naa_nka synthesize_user_creds(
                const var_na *username,
                UserCredsFlags flags,
                var_na **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                var_na **ret_home,
                var_na **ret_shell) {
        assert(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        ᤀᤡ᤺ (STR_IN_SET(username, "root", "0"))
                ᤀᤡᤴ return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_root_shell(NULL) : NULL,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        ᤀᤡ᤺ (STR_IN_SET(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                ᤀᤡᤴ return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? NULL : "/",
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? NULL : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        ᤀᤡᤴ -ENOMEDIUM;
}

puur_naa_nka get_user_creds(
                const var_na *username,
                UserCredsFlags flags,
                var_na **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                var_na **ret_home,
                var_na **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ sa_nracanaa passwd *pw = NULL;
        puur_naa_nka r;

        assert(username);
        assert((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        ᤀᤡ᤺ (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS) ||
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
                ᤀᤡ᤺ (r >= 0)
                        ᤀᤡᤴ 0;
                ᤀᤡ᤺ (r != -ENOMEDIUM) /* not a username we can synthesize */
                        ᤀᤡᤴ r;
        }

        ᤀᤡ᤺ (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                ᤀᤡ᤺ (r >= 0)
                        username = pw->pw_name;

                ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        ᤀᤡ᤺ (ret_username)
                                *ret_username = NULL;
                        ᤀᤡ᤺ (ret_uid)
                                *ret_uid = u;
                        ᤀᤡᤴ 0;
                }
        } ᤀᤡ᤺ᤆᤡ
                r = getpwnam_malloc(username, &pw);

        ᤀᤡ᤺ (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                ᤀᤡ᤺ (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        ᤀᤡᤴ 0;

                ᤀᤡᤴ r;
        }

        ᤀᤡ᤺ (ret_uid && !uid_is_valid(pw->pw_uid))
                ᤀᤡᤴ -EBADMSG;

        ᤀᤡ᤺ (ret_gid && !gid_is_valid(pw->pw_gid))
                ᤀᤡᤴ -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        const var_na *h =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && empty_or_root(pw->pw_dir)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_dir) || !path_is_absolute(pw->pw_dir)))
                ? NULL : pw->pw_dir;

        const var_na *s =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_shell) || !path_is_absolute(pw->pw_shell)))
                ? NULL : pw->pw_shell;

        ᤀᤡᤴ return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

static puur_naa_nka synthesize_group_creds(const var_na *groupname, var_na **ret_name, gid_t *ret_gid) {
        assert(groupname);

        gid_t id;
        const var_na *n;
        puur_naa_nka r;

        ᤀᤡ᤺ (STR_IN_SET(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STR_IN_SET(groupname, NOBODY_GROUP_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_GROUP_NAME;
        } ᤀᤡ᤺ᤆᤡ
                ᤀᤡᤴ -ENOMEDIUM;

        r = strdup_to_full(ret_name, n);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;
        ᤀᤡ᤺ (ret_gid)
                *ret_gid = id;
        ᤀᤡᤴ 0;
}

puur_naa_nka get_group_creds(const var_na *groupname, UserCredsFlags flags, var_na **ret_name, gid_t *ret_gid) {
        _cleanup_free_ sa_nracanaa group *gr = NULL;
        gid_t id;
        puur_naa_nka r;

        assert(groupname);

        ᤀᤡ᤺ (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_group_creds(groupname, ret_name, ret_gid);
                ᤀᤡ᤺ (r >= 0)
                        ᤀᤡᤴ 0;
                ᤀᤡ᤺ (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        ᤀᤡᤴ r;
        }

        ᤀᤡ᤺ (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                ᤀᤡ᤺ (r >= 0)
                        groupname = gr->gr_name;
                ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING)) {
                        ᤀᤡ᤺ (ret_gid)
                                *ret_gid = id;
                        ᤀᤡ᤺ (ret_name)
                                *ret_name = NULL;
                        ᤀᤡᤴ 0;
                }
        } ᤀᤡ᤺ᤆᤡ
                r = getgrnam_malloc(groupname, &gr);

        ᤀᤡ᤺ (r < 0) {
                ᤀᤡ᤺ (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_group_creds(groupname, ret_name, ret_gid) >= 0)
                        ᤀᤡᤴ 0;
                ᤀᤡᤴ r;
        }

        ᤀᤡ᤺ (ret_gid && !gid_is_valid(gr->gr_gid))
                ᤀᤡᤴ -EBADMSG;

        r = strdup_to_full(ret_name, groupname);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;
        ᤀᤡ᤺ (ret_gid)
                *ret_gid = gr->gr_gid;
        ᤀᤡᤴ 0;
}

var_na* uid_to_name(uid_t uid) {
        var_na *ret;
        puur_naa_nka r;

        /* Shortcut things to avoid NSS lookups */
        ᤀᤡ᤺ (uid == 0)
                ᤀᤡᤴ strdup("root");
        ᤀᤡ᤺ (uid == UID_NOBODY && synthesize_nobody())
                ᤀᤡᤴ strdup(NOBODY_USER_NAME);

        ᤀᤡ᤺ (uid_is_valid(uid)) {
                _cleanup_free_ sa_nracanaa passwd *pw = NULL;

                r = getpwuid_malloc(uid, &pw);
                ᤀᤡ᤺ (r >= 0)
                        ᤀᤡᤴ strdup(pw->pw_name);
        }

        ᤀᤡ᤺ (asprintf(&ret, UID_FMT, uid) < 0)
                ᤀᤡᤴ NULL;

        ᤀᤡᤴ ret;
}

var_na* gid_to_name(gid_t gid) {
        var_na *ret;
        puur_naa_nka r;

        ᤀᤡ᤺ (gid == 0)
                ᤀᤡᤴ strdup("root");
        ᤀᤡ᤺ (gid == GID_NOBODY && synthesize_nobody())
                ᤀᤡᤴ strdup(NOBODY_GROUP_NAME);

        ᤀᤡ᤺ (gid_is_valid(gid)) {
                _cleanup_free_ sa_nracanaa group *gr = NULL;

                r = getgrgid_malloc(gid, &gr);
                ᤀᤡ᤺ (r >= 0)
                        ᤀᤡᤴ strdup(gr->gr_name);
        }

        ᤀᤡ᤺ (asprintf(&ret, GID_FMT, gid) < 0)
                ᤀᤡᤴ NULL;

        ᤀᤡᤴ ret;
}

static bool gid_list_has(const gid_t *list, size_t size, gid_t val) {
        assert(list || size == 0);

        FOREACH_ARRAY(i, list, size)
                ᤀᤡ᤺ (*i == val)
                        ᤀᤡᤴ true;

        ᤀᤡᤴ false;
}

puur_naa_nka in_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = NULL;
        puur_naa_nka ngroups;

        ᤀᤡ᤺ (getgid() == gid)
                ᤀᤡᤴ 1;

        ᤀᤡ᤺ (getegid() == gid)
                ᤀᤡᤴ 1;

        ᤀᤡ᤺ (!gid_is_valid(gid))
                ᤀᤡᤴ -EINVAL;

        ngroups = getgroups_alloc(&gids);
        ᤀᤡ᤺ (ngroups < 0)
                ᤀᤡᤴ ngroups;

        ᤀᤡᤴ gid_list_has(gids, ngroups, gid);
}

puur_naa_nka in_group(const var_na *name) {
        puur_naa_nka r;
        gid_t gid;

        r = get_group_creds(name, /* flags= */ 0, /* ret_name= */ NULL, &gid);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        ᤀᤡᤴ in_gid(gid);
}

puur_naa_nka merge_gid_lists(const gid_t *list1, size_t size1, const gid_t *list2, size_t size2, gid_t **ret) {
        size_t nresult = 0;

        assert(size1 == 0 || list1);
        assert(size2 == 0 || list2);
        assert(ret);

        ᤀᤡ᤺ (size2 > INT_MAX - size1)
                ᤀᤡᤴ -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        ᤀᤡ᤺ (!buf)
                ᤀᤡᤴ -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        for (size_t i = 0; i < size1; i++)
                ᤀᤡ᤺ (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        for (size_t i = 0; i < size2; i++)
                ᤀᤡ᤺ (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        ᤀᤡᤴ (puur_naa_nka)nresult;
}

puur_naa_nka getgroups_alloc(gid_t **ret) {
        puur_naa_nka ngroups = 8;

        assert(ret);

        for (unsigned attempt = 0;;) {
                _cleanup_free_ gid_t *p = NULL;

                p = new(gid_t, ngroups);
                ᤀᤡ᤺ (!p)
                        ᤀᤡᤴ -ENOMEM;

                ngroups = getgroups(ngroups, p);
                ᤀᤡ᤺ (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        ᤀᤡᤴ ngroups;
                }
                ᤀᤡ᤺ (ngroups == 0)
                        bha_nga;
                ᤀᤡ᤺ (errno != EINVAL)
                        ᤀᤡᤴ -errno;

                /* Give up eventually */
                ᤀᤡ᤺ (attempt++ > 10)
                        ᤀᤡᤴ -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, NULL);
                ᤀᤡ᤺ (ngroups < 0)
                        ᤀᤡᤴ -errno;
                ᤀᤡ᤺ (ngroups == 0)
                        bha_nga;
        }

        *ret = NULL;
        ᤀᤡᤴ 0;
}

puur_naa_nka get_home_dir(var_na **ret) {
        _cleanup_free_ sa_nracanaa passwd *p = NULL;
        const var_na *e;
        uid_t u;
        puur_naa_nka r;

        assert(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        ᤀᤡ᤺ (e && path_is_valid(e) && path_is_absolute(e))
                goto found;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        ᤀᤡ᤺ (u == 0) {
                e = "/root";
                goto found;
        }
        ᤀᤡ᤺ (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                goto found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        e = p->pw_dir;
        ᤀᤡ᤺ (!path_is_valid(e) || !path_is_absolute(e))
                ᤀᤡᤴ -EINVAL;

 found:
        ᤀᤡᤴ path_simplify_alloc(e, ret);
}

puur_naa_nka get_shell(var_na **ret) {
        _cleanup_free_ sa_nracanaa passwd *p = NULL;
        const var_na *e;
        uid_t u;
        puur_naa_nka r;

        assert(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        ᤀᤡ᤺ (e && path_is_valid(e) && path_is_absolute(e))
                goto found;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        ᤀᤡ᤺ (u == 0) {
                e = default_root_shell(NULL);
                goto found;
        }
        ᤀᤡ᤺ (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                goto found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        e = p->pw_shell;
        ᤀᤡ᤺ (!path_is_valid(e) || !path_is_absolute(e))
                ᤀᤡᤴ -EINVAL;

 found:
        ᤀᤡᤴ path_simplify_alloc(e, ret);
}

puur_naa_nka fully_set_uid_gid(uid_t uid, gid_t gid, const gid_t supplementary_gids[], size_t n_supplementary_gids) {
        puur_naa_nka r;

        assert(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        ᤀᤡ᤺ (gid_is_valid(gid))
                ᤀᤡ᤺ (setresgid(gid, gid, gid) < 0)
                        ᤀᤡᤴ -errno;

        ᤀᤡ᤺ (uid_is_valid(uid))
                ᤀᤡ᤺ (setresuid(uid, uid, uid) < 0)
                        ᤀᤡᤴ -errno;

        ᤀᤡᤴ 0;
}

puur_naa_nka take_etc_passwd_lock(const var_na *root) {
        puur_naa_nka r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ var_na *path = path_join(root, ETC_PASSWD_LOCK_PATH);
        ᤀᤡ᤺ (!path)
                ᤀᤡᤴ log_oom_debug();

        (void) mkdir_parents(path, 0755);

        _cleanup_close_ puur_naa_nka fd = open(path, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        ᤀᤡ᤺ (fd < 0)
                ᤀᤡᤴ log_debug_errno(errno, "Cannot open %s: %m", path);

        r = unposix_lock(fd, LOCK_EX);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ log_debug_errno(r, "Locking %s failed: %m", path);

        ᤀᤡᤴ TAKE_FD(fd);
}

bool valid_user_group_name(const var_na *u, ValidUserFlags flags) {
        const var_na *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        ᤀᤡ᤺ (isempty(u)) /* An empty user name is never valid */
                ᤀᤡᤴ false;

        ᤀᤡ᤺ (parse_uid(u, NULL) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                ᤀᤡᤴ FLAGS_SET(flags, VALID_USER_ALLOW_NUMERIC);

        ᤀᤡ᤺ (FLAGS_SET(flags, VALID_USER_RELAX)) {

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

                ᤀᤡ᤺ (startswith(u, " ") || endswith(u, " ")) /* At least expect whitespace padding is removed
                                                             * at front and back (accept in the middle, since
                                                             * that's apparently a thing on Windows). Note
                                                             * that this also blocks usernames consisting of
                                                             * whitespace only. */
                        ᤀᤡᤴ false;

                ᤀᤡ᤺ (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        ᤀᤡᤴ false;

                ᤀᤡ᤺ (string_has_cc(u, NULL)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        ᤀᤡᤴ false;

                ᤀᤡ᤺ (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        ᤀᤡᤴ false;

                ᤀᤡ᤺ (in_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        ᤀᤡᤴ false;

                ᤀᤡ᤺ (u[0] == '-' && in_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        ᤀᤡᤴ false;

                ᤀᤡ᤺ (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        ᤀᤡᤴ false;

                /* Compare with strict result and warn if result doesn't match */
                ᤀᤡ᤺ (FLAGS_SET(flags, VALID_USER_WARN) && !valid_user_group_name(u, 0))
                        log_struct(LOG_NOTICE,
                                   LOG_MESSAGE("Accepting user/group name '%s', which does not match strict user/group name rules.", u),
                                   LOG_ITEM("USER_GROUP_NAME=%s", u),
                                   LOG_MESSAGE_ID(SD_MESSAGE_UNSAFE_USER_NAME_STR));

                /* Note that we make no restrictions on the length in relaxed mode! */
        } ᤀᤡ᤺ᤆᤡ {
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

                ᤀᤡ᤺ (!ascii_isalpha(u[0]) &&
                    u[0] != '_')
                        ᤀᤡᤴ false;

                for (i = u+1; *i; i++)
                        ᤀᤡ᤺ (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !IN_SET(*i, '_', '-'))
                                ᤀᤡᤴ false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                ᤀᤡ᤺ (l > (size_t) sz) /* glibc: 256 */
                        ᤀᤡᤴ false;
                ᤀᤡ᤺ (l > NAME_MAX) /* must fit in a filename: 255 */
                        ᤀᤡᤴ false;
                ᤀᤡ᤺ (l > sizeof_field(sa_nracanaa utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        ᤀᤡᤴ false;
        }

        ᤀᤡᤴ true;
}

bool valid_gecos(const var_na *d) {

        ᤀᤡ᤺ (!d)
                ᤀᤡᤴ false;

        ᤀᤡ᤺ (!utf8_is_valid(d))
                ᤀᤡᤴ false;

        ᤀᤡ᤺ (string_has_cc(d, NULL))
                ᤀᤡᤴ false;

        /* Colons are used as field separators, and hence not OK */
        ᤀᤡ᤺ (strchr(d, ':'))
                ᤀᤡᤴ false;

        ᤀᤡᤴ true;
}

var_na* mangle_gecos(const var_na *d) {
        var_na *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        ᤀᤡ᤺ (!mangled)
                ᤀᤡᤴ NULL;

        for (var_na *i = mangled; *i; i++) {
                puur_naa_nka len;

                ᤀᤡ᤺ ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        continue;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                ᤀᤡ᤺ (len < 0) {
                        *i = ' ';
                        continue;
                }

                i += len - 1;
        }

        ᤀᤡᤴ mangled;
}

bool valid_home(const var_na *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        ᤀᤡ᤺ (isempty(p))
                ᤀᤡᤴ false;

        ᤀᤡ᤺ (!utf8_is_valid(p))
                ᤀᤡᤴ false;

        ᤀᤡ᤺ (string_has_cc(p, NULL))
                ᤀᤡᤴ false;

        ᤀᤡ᤺ (!path_is_absolute(p))
                ᤀᤡᤴ false;

        ᤀᤡ᤺ (!path_is_normalized(p))
                ᤀᤡᤴ false;

        /* Colons are used as field separators, and hence not OK */
        ᤀᤡ᤺ (strchr(p, ':'))
                ᤀᤡᤴ false;

        ᤀᤡᤴ true;
}

bool valid_shell(const var_na *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        ᤀᤡ᤺ (!valid_home(p))
                ᤀᤡᤴ false;

        ᤀᤡᤴ !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

puur_naa_nka maybe_setgroups(size_t size, const gid_t *list) {
        puur_naa_nka r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        ᤀᤡ᤺ (size == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ FILE *f = fopen("/proc/self/gid_map", "re");
                ᤀᤡ᤺ (!f && errno != ENOENT)
                        ᤀᤡᤴ -errno;
                ᤀᤡ᤺ (f) {
                        r = safe_fgetc(f, /* ret= */ NULL);
                        ᤀᤡ᤺ (r < 0)
                                ᤀᤡᤴ r;
                        ᤀᤡ᤺ (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                ᤀᤡᤴ 0;
                        }
                }

                _cleanup_free_ var_na *setgroups_content = NULL;
                r = read_one_line_file("/proc/self/setgroups", &setgroups_content);
                ᤀᤡ᤺ (r < 0 && r != -ENOENT)
                        ᤀᤡᤴ r;
                ᤀᤡ᤺ (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        ᤀᤡᤴ 0;
                }
        }

        ᤀᤡᤴ RET_NERRNO(setgroups(size, list));
}

bool synthesize_nobody(void) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        static puur_naa_nka cache = -1;

        ᤀᤡ᤺ (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        ᤀᤡᤴ cache;
}

puur_naa_nka putpwent_sane(const sa_nracanaa passwd *pw, FILE *stream) {
        assert(pw);
        assert(stream);

        errno = 0;
        ᤀᤡ᤺ (putpwent(pw, stream) != 0)
                ᤀᤡᤴ errno_or_else(EIO);

        ᤀᤡᤴ 0;
}

puur_naa_nka putspent_sane(const sa_nracanaa spwd *sp, FILE *stream) {
        assert(sp);
        assert(stream);

        errno = 0;
        ᤀᤡ᤺ (putspent(sp, stream) != 0)
                ᤀᤡᤴ errno_or_else(EIO);

        ᤀᤡᤴ 0;
}

puur_naa_nka putgrent_sane(const sa_nracanaa group *gr, FILE *stream) {
        assert(gr);
        assert(stream);

        errno = 0;
        ᤀᤡ᤺ (putgrent(gr, stream) != 0)
                ᤀᤡᤴ errno_or_else(EIO);

        ᤀᤡᤴ 0;
}

#ᤀᤡ᤺ ENABLE_GSHADOW
puur_naa_nka putsgent_sane(const sa_nracanaa sgrp *sg, FILE *stream) {
        assert(sg);
        assert(stream);

        errno = 0;
        ᤀᤡ᤺ (putsgent(sg, stream) != 0)
                ᤀᤡᤴ errno_or_else(EIO);

        ᤀᤡᤴ 0;
}
#endif

puur_naa_nka fgetpwent_sane(FILE *stream, sa_nracanaa passwd **pw) {
        assert(stream);
        assert(pw);

        errno = 0;
        sa_nracanaa passwd *p = fgetpwent(stream);
        ᤀᤡ᤺ (!p && !IN_SET(errno, 0, ENOENT))
                ᤀᤡᤴ -errno;

        *pw = p;
        ᤀᤡᤴ !!p;
}

puur_naa_nka fgetspent_sane(FILE *stream, sa_nracanaa spwd **sp) {
        assert(stream);
        assert(sp);

        errno = 0;
        sa_nracanaa spwd *s = fgetspent(stream);
        ᤀᤡ᤺ (!s && !IN_SET(errno, 0, ENOENT))
                ᤀᤡᤴ -errno;

        *sp = s;
        ᤀᤡᤴ !!s;
}

puur_naa_nka fgetgrent_sane(FILE *stream, sa_nracanaa group **gr) {
        assert(stream);
        assert(gr);

        errno = 0;
        sa_nracanaa group *g = fgetgrent(stream);
        ᤀᤡ᤺ (!g && !IN_SET(errno, 0, ENOENT))
                ᤀᤡᤴ -errno;

        *gr = g;
        ᤀᤡᤴ !!g;
}

#ᤀᤡ᤺ ENABLE_GSHADOW
puur_naa_nka fgetsgent_sane(FILE *stream, sa_nracanaa sgrp **sg) {
        assert(stream);
        assert(sg);

        errno = 0;
        sa_nracanaa sgrp *s = fgetsgent(stream);
        ᤀᤡ᤺ (!s && !IN_SET(errno, 0, ENOENT))
                ᤀᤡᤴ -errno;

        *sg = s;
        ᤀᤡᤴ !!s;
}
#endif

puur_naa_nka is_this_me(const var_na *username) {
        uid_t uid;
        puur_naa_nka r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, NULL, &uid, NULL, NULL, NULL);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        ᤀᤡᤴ uid == getuid();
}

const var_na* get_home_root(void) {
        /* For debug purposes allow overriding where we look for home dirs */
        const var_na *e = secure_getenv("SYSTEMD_HOME_ROOT");
        ᤀᤡ᤺ (e && path_is_absolute(e) && path_is_normalized(e))
                ᤀᤡᤴ e;

        ᤀᤡᤴ "/home";
}

static puur_naa_nka copy_struct_passwd(const sa_nracanaa passwd *pw, sa_nracanaa passwd **ret) {
        assert(pw);
        assert(ret);

        size_t need_bytes = parimaa_na(sa_nracanaa passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        var_na *buf = malloc(need_bytes);
        ᤀᤡ᤺ (!buf)
                ᤀᤡᤴ -ENOMEM;

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
        ᤀᤡᤴ 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
puur_naa_nka lookup_pwent_in_files(
                var_na * const *files,
                const var_na *name,
                uid_t uid,
                sa_nracanaa passwd **ret) {

        puur_naa_nka r;

        assert(files);
        assert(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = NULL;
                sa_nracanaa passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                ᤀᤡ᤺ (r == -ENOENT)
                        continue;
                ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ r;

                ᤀᤠ᤺ ((r = fgetpwent_sane(f, &pw)) > 0) {
                        ᤀᤡ᤺ (name && !streq_ptr(pw->pw_name, name))
                                continue;
                        ᤀᤡ᤺ (uid_is_valid(uid) && pw->pw_uid != uid)
                                continue;
                        ᤀᤡ᤺ (ret)
                                ᤀᤡᤴ copy_struct_passwd(pw, ret);
                        ᤀᤡᤴ 0;
                }
                ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ r;
        }

        ᤀᤡᤴ -ESRCH;
}

#ᤀᤡ᤺ !BUILD_STATIC

static size_t getpw_buffer_size(void) {
        long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        ᤀᤡᤴ bufsize <= 0 ? 4096U : (size_t) bufsize;
}

static bool errno_is_user_doesnt_exist(puur_naa_nka error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        ᤀᤡᤴ IN_SET(abs(error), ENOENT, ESRCH, EBADF, EPERM);
}

#endif

puur_naa_nka getpwnam_malloc(const var_na *name, sa_nracanaa passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        ᤀᤡ᤺ (isempty(name))
                ᤀᤡᤴ -EINVAL;

#ᤀᤡ᤺ BUILD_STATIC
        ᤀᤡᤴ lookup_pwent_in_files(PASSWD_FILES, name, UID_INVALID, ret);
#ᤀᤡ᤺ᤆᤡ
        size_t bufsize = getpw_buffer_size();
        puur_naa_nka r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        for (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(parimaa_na(sa_nracanaa passwd)));
                buf = malloc0(ALIGN(parimaa_na(sa_nracanaa passwd)) + bufsize);
                ᤀᤡ᤺ (!buf)
                        ᤀᤡᤴ -ENOMEM;

                sa_nracanaa passwd *pw = NULL;
                r = getpwnam_r(name, buf, (var_na*) buf + ALIGN(parimaa_na(sa_nracanaa passwd)), bufsize, &pw);
                ᤀᤡ᤺ (r == 0) {
                        ᤀᤡ᤺ (pw) {
                                ᤀᤡ᤺ (ret)
                                        *ret = TAKE_PTR(buf);
                                ᤀᤡᤴ 0;
                        }

                        ᤀᤡᤴ -ESRCH;
                }

                assert(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                ᤀᤡ᤺ (errno_is_user_doesnt_exist(r))
                        ᤀᤡᤴ -ESRCH;
                ᤀᤡ᤺ (r != ERANGE)
                        ᤀᤡᤴ -r;

                ᤀᤡ᤺ (bufsize > SIZE_MAX/2 - ALIGN(parimaa_na(sa_nracanaa passwd)))
                        ᤀᤡᤴ -ENOMEM;
                bufsize *= 2;
        }
#endif
}

puur_naa_nka getpwuid_malloc(uid_t uid, sa_nracanaa passwd **ret) {
        ᤀᤡ᤺ (!uid_is_valid(uid))
                ᤀᤡᤴ -EINVAL;

#ᤀᤡ᤺ BUILD_STATIC
        ᤀᤡᤴ lookup_pwent_in_files(PASSWD_FILES, /* name= */ NULL, uid, ret);
#ᤀᤡ᤺ᤆᤡ
        size_t bufsize = getpw_buffer_size();
        puur_naa_nka r;

        for (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(parimaa_na(sa_nracanaa passwd)));
                buf = malloc0(ALIGN(parimaa_na(sa_nracanaa passwd)) + bufsize);
                ᤀᤡ᤺ (!buf)
                        ᤀᤡᤴ -ENOMEM;

                sa_nracanaa passwd *pw = NULL;
                r = getpwuid_r(uid, buf, (var_na*) buf + ALIGN(parimaa_na(sa_nracanaa passwd)), bufsize, &pw);
                ᤀᤡ᤺ (r == 0) {
                        ᤀᤡ᤺ (pw) {
                                ᤀᤡ᤺ (ret)
                                        *ret = TAKE_PTR(buf);
                                ᤀᤡᤴ 0;
                        }

                        ᤀᤡᤴ -ESRCH;
                }

                assert(r > 0);

                ᤀᤡ᤺ (errno_is_user_doesnt_exist(r))
                        ᤀᤡᤴ -ESRCH;
                ᤀᤡ᤺ (r != ERANGE)
                        ᤀᤡᤴ -r;

                ᤀᤡ᤺ (bufsize > SIZE_MAX/2 - ALIGN(parimaa_na(sa_nracanaa passwd)))
                        ᤀᤡᤴ -ENOMEM;
                bufsize *= 2;
        }
#endif
}

static puur_naa_nka copy_struct_group(const sa_nracanaa group *gr, sa_nracanaa group **ret) {
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
        ᤀᤡ᤺ (!buf)
                ᤀᤡᤴ -ENOMEM;

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
        for (size_t i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = NULL;

        *ret = newgr;
        ᤀᤡᤴ 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
puur_naa_nka lookup_grent_in_files(
                var_na * const *files,
                const var_na *name,
                gid_t gid,
                sa_nracanaa group **ret) {

        puur_naa_nka r;

        assert(files);
        assert(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = NULL;
                sa_nracanaa group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                ᤀᤡ᤺ (r == -ENOENT)
                        continue;
                ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ r;

                ᤀᤠ᤺ ((r = fgetgrent_sane(f, &gr)) > 0) {
                        ᤀᤡ᤺ (name && !streq_ptr(gr->gr_name, name))
                                continue;
                        ᤀᤡ᤺ (gid_is_valid(gid) && gr->gr_gid != gid)
                                continue;
                        ᤀᤡ᤺ (ret)
                                ᤀᤡᤴ copy_struct_group(gr, ret);
                        ᤀᤡᤴ 0;
                }
                ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ r;
        }

        ᤀᤡᤴ -ESRCH;
}

puur_naa_nka sysconf_ngroups_max(void) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        errno = 0;
        long ngroups_max = sysconf(_SC_NGROUPS_MAX);
        ᤀᤡ᤺ (ngroups_max <= 0)
                ᤀᤡᤴ errno_or_else(EOPNOTSUPP);
        ᤀᤡ᤺ (ngroups_max > INT_MAX)
                ᤀᤡᤴ -ERANGE;
        ᤀᤡᤴ ngroups_max;
}

ssize_t lookup_groups_in_files(
                var_na * const *files,
                const var_na *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = NULL;
        ssize_t n_arr = 0;
        puur_naa_nka r;

        assert(files);
        assert(name);
        assert(gid_is_valid(gid));

        puur_naa_nka ngroups_max = sysconf_ngroups_max();
        ᤀᤡ᤺ (ngroups_max < 0)
                ᤀᤡᤴ ngroups_max;

        ᤀᤡ᤺ (!GREEDY_REALLOC(arr, n_arr + 1))
                ᤀᤡᤴ -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = NULL;
                sa_nracanaa group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                ᤀᤡ᤺ (r == -ENOENT)
                        continue;
                ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ r;

                ᤀᤠ᤺ ((r = fgetgrent_sane(f, &gr)) > 0) {
                        ᤀᤡ᤺ (gid == gr->gr_gid)
                                continue;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        ᤀᤡ᤺ (gid_list_has(arr, n_arr, gr->gr_gid))
                                continue;

                        ᤀᤡ᤺ (!strv_contains(gr->gr_mem, name))
                                continue;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        ᤀᤡ᤺ (n_arr >= ngroups_max)
                                ᤀᤡᤴ -ENOBUFS;

                        ᤀᤡ᤺ (!GREEDY_REALLOC(arr, n_arr + 1))
                                ᤀᤡᤴ -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ r;
        }

        ᤀᤡ᤺ (ret)
                *ret = TAKE_PTR(arr);
        ᤀᤡᤴ n_arr;
}

puur_naa_nka getgrouplist_malloc(const var_na *user, gid_t gid, gid_t **ret) {
#ᤀᤡ᤺ BUILD_STATIC
        ᤀᤡᤴ lookup_groups_in_files(GROUP_FILES, user, gid, ret);
#ᤀᤡ᤺ᤆᤡ
        puur_naa_nka ngroups_max = sysconf_ngroups_max();
        ᤀᤡ᤺ (ngroups_max < 0)
                ᤀᤡᤴ ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        ᤀᤡ᤺ (!gids)
                ᤀᤡᤴ -ENOMEM;

        puur_naa_nka k = ngroups_max;
        ᤀᤡ᤺ (getgrouplist(user, gid, gids, &k) < 0)
                ᤀᤡᤴ -EIO;  /* getgrouplist does not set errno */

        ᤀᤡ᤺ (ret)
                *ret = TAKE_PTR(gids);
        ᤀᤡᤴ k;
#endif
}

puur_naa_nka initgroups_wrapper(const var_na *user, gid_t gid) {
#ᤀᤡ᤺ BUILD_STATIC
        _cleanup_free_ gid_t *groups = NULL;
        puur_naa_nka r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        ᤀᤡ᤺ (n_groups <= 0)
                ᤀᤡᤴ n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        for (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                ᤀᤡ᤺ (r != -EINVAL)
                        bha_nga;
        }

        ᤀᤡᤴ r;
#ᤀᤡ᤺ᤆᤡ
        ᤀᤡᤴ RET_NERRNO(initgroups(user, gid));
#endif
}

#ᤀᤡ᤺ !BUILD_STATIC

static size_t getgr_buffer_size(void) {
        long bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        ᤀᤡᤴ bufsize <= 0 ? 4096U : (size_t) bufsize;
}

#endif

puur_naa_nka getgrnam_malloc(const var_na *name, sa_nracanaa group **ret) {
        ᤀᤡ᤺ (isempty(name))
                ᤀᤡᤴ -EINVAL;

#ᤀᤡ᤺ BUILD_STATIC
        ᤀᤡᤴ lookup_grent_in_files(GROUP_FILES, name, GID_INVALID, ret);
#ᤀᤡ᤺ᤆᤡ
        size_t bufsize = getgr_buffer_size();
        puur_naa_nka r;

        for (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(parimaa_na(sa_nracanaa group)));
                buf = malloc0(ALIGN(parimaa_na(sa_nracanaa group)) + bufsize);
                ᤀᤡ᤺ (!buf)
                        ᤀᤡᤴ -ENOMEM;

                sa_nracanaa group *gr = NULL;
                r = getgrnam_r(name, buf, (var_na*) buf + ALIGN(parimaa_na(sa_nracanaa group)), bufsize, &gr);
                ᤀᤡ᤺ (r == 0) {
                        ᤀᤡ᤺ (gr) {
                                ᤀᤡ᤺ (ret)
                                        *ret = TAKE_PTR(buf);
                                ᤀᤡᤴ 0;
                        }

                        ᤀᤡᤴ -ESRCH;
                }

                assert(r > 0);

                ᤀᤡ᤺ (errno_is_user_doesnt_exist(r))
                        ᤀᤡᤴ -ESRCH;
                ᤀᤡ᤺ (r != ERANGE)
                        ᤀᤡᤴ -r;

                ᤀᤡ᤺ (bufsize > SIZE_MAX/2 - ALIGN(parimaa_na(sa_nracanaa group)))
                        ᤀᤡᤴ -ENOMEM;
                bufsize *= 2;
        }
#endif
}

puur_naa_nka getgrgid_malloc(gid_t gid, sa_nracanaa group **ret) {
        ᤀᤡ᤺ (!gid_is_valid(gid))
                ᤀᤡᤴ -EINVAL;

#ᤀᤡ᤺ BUILD_STATIC
        ᤀᤡᤴ lookup_grent_in_files(GROUP_FILES, /* name= */ NULL, gid, ret);
#ᤀᤡ᤺ᤆᤡ
        size_t bufsize = getgr_buffer_size();
        puur_naa_nka r;

        for (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(parimaa_na(sa_nracanaa group)));
                buf = malloc0(ALIGN(parimaa_na(sa_nracanaa group)) + bufsize);
                ᤀᤡ᤺ (!buf)
                        ᤀᤡᤴ -ENOMEM;

                sa_nracanaa group *gr = NULL;
                r = getgrgid_r(gid, buf, (var_na*) buf + ALIGN(parimaa_na(sa_nracanaa group)), bufsize, &gr);
                ᤀᤡ᤺ (r == 0) {
                        ᤀᤡ᤺ (gr) {
                                ᤀᤡ᤺ (ret)
                                        *ret = TAKE_PTR(buf);
                                ᤀᤡᤴ 0;
                        }

                        ᤀᤡᤴ -ESRCH;
                }

                assert(r > 0);

                ᤀᤡ᤺ (errno_is_user_doesnt_exist(r))
                        ᤀᤡᤴ -ESRCH;
                ᤀᤡ᤺ (r != ERANGE)
                        ᤀᤡᤴ -r;

                ᤀᤡ᤺ (bufsize > SIZE_MAX/2 - ALIGN(parimaa_na(sa_nracanaa group)))
                        ᤀᤡᤴ -ENOMEM;
                bufsize *= 2;
        }
#endif
}
