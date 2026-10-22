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
                        ចំនួនគត់ errnum,                                     \
                        var_na *buf,                                      \
                        size_t buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                switch (errnum) {                                       \
                pak_sa ESRCH:                                             \
                        ត្រឡប់ "Unknown " STRINGIFY(type);              \
                pak_sa ENOEXEC:                                           \
                        ត្រឡប់ "Not a system " STRINGIFY(type);         \
                default:                                                \
                        ត្រឡប់ strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        ប្រសិនបើ (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                ត្រឡប់ false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        ប្រសិនបើ (uid == (uid_t) UINT32_C(0xFFFF))
                ត្រឡប់ false;

        ត្រឡប់ true;
}

ចំនួនគត់ parse_uid(const var_na *s, uid_t *ret) {
        uint32_t uid = 0;
        ចំនួនគត់ r;

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
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (!uid_is_valid(uid))
                ត្រឡប់ -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        ប្រសិនបើ (ret)
                *ret = uid;

        ត្រឡប់ 0;
}

ចំនួនគត់ parse_uid_range(const var_na *s, uid_t *ret_lower, uid_t *ret_upper) {
        _cleanup_free_ var_na *word = NULL;
        uid_t l, u;
        ចំនួនគត់ r;

        assert(s);
        assert(ret_lower);
        assert(ret_upper);

        r = extract_first_word(&s, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;
        ប្រសិនបើ (r == 0)
                ត្រឡប់ -EINVAL;

        r = parse_uid(word, &l);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        /* Check for the upper bound and extract it if needed */
        ប្រសិនបើ (!s)
                /* Single number with no dash. */
                u = l;
        បើមិនដូច្នេះ ប្រសិនបើ (!*s)
                /* Trailing dash is an error. */
                ត្រឡប់ -EINVAL;
        បើមិនដូច្នេះ {
                r = parse_uid(s, &u);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                ប្រសិនបើ (l > u)
                        ត្រឡប់ -EINVAL;
        }

        *ret_lower = l;
        *ret_upper = u;
        ត្រឡប់ 0;
}

var_na* getlogname_malloc(void) {
        uid_t uid;
        sa_nracanaa stat st;

        ប្រសិនបើ (isatty_safe(STDIN_FILENO) && fstat(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        បើមិនដូច្នេះ
                uid = getuid();

        ត្រឡប់ uid_to_name(uid);
}

var_na* getusername_malloc(void) {
        const var_na *e;

        e = secure_getenv("USER");
        ប្រសិនបើ (e)
                ត្រឡប់ strdup(e);

        ត្រឡប់ uid_to_name(getuid());
}

bool is_nologin_shell(const var_na *shell) {
        ត្រឡប់ PATH_IN_SET(shell,
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
        ត្រឡប់ isempty(shell) || is_nologin_shell(shell);
}

const var_na* default_root_shell_at(ចំនួនគត់ rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        assert(rfd >= 0 || rfd == AT_FDCWD);

        ចំនួនគត់ r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, NULL, NULL);
        ប្រសិនបើ (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        ប្រសិនបើ (r > 0)
                ត្រឡប់ DEFAULT_USER_SHELL;

        ត្រឡប់ "/bin/sh";
}

const var_na* default_root_shell(const var_na *root) {
        _cleanup_close_ ចំនួនគត់ rfd = -EBADF;

        rfd = open(empty_to_root(root), O_CLOEXEC | O_DIRECTORY | O_PATH);
        ប្រសិនបើ (rfd < 0)
                ត្រឡប់ "/bin/sh";

        ត្រឡប់ default_root_shell_at(rfd);
}

static ចំនួនគត់ return_user_creds(
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
        ចំនួនគត់ r;

        ប្រសិនបើ (ret_username) {
                r = strdup_to(&s1, username);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
        }

        ប្រសិនបើ (ret_home) {
                r = strdup_to(&s2, home);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
        }

        ប្រសិនបើ (ret_shell) {
                r = strdup_to(&s3, shell);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
        }

        ប្រសិនបើ (ret_username)
                *ret_username = TAKE_PTR(s1);
        ប្រសិនបើ (ret_uid)
                *ret_uid = uid;
        ប្រសិនបើ (ret_gid)
                *ret_gid = gid;
        ប្រសិនបើ (ret_home)
                *ret_home = TAKE_PTR(s2);
        ប្រសិនបើ (ret_shell)
                *ret_shell = TAKE_PTR(s3);
        ត្រឡប់ 0;
}

static ចំនួនគត់ synthesize_user_creds(
                const var_na *username,
                UserCredsFlags flags,
                var_na **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                var_na **ret_home,
                var_na **ret_shell) {
        assert(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        ប្រសិនបើ (STR_IN_SET(username, "root", "0"))
                ត្រឡប់ return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_root_shell(NULL) : NULL,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        ប្រសិនបើ (STR_IN_SET(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                ត្រឡប់ return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? NULL : "/",
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? NULL : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        ត្រឡប់ -ENOMEDIUM;
}

ចំនួនគត់ get_user_creds(
                const var_na *username,
                UserCredsFlags flags,
                var_na **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                var_na **ret_home,
                var_na **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ sa_nracanaa passwd *pw = NULL;
        ចំនួនគត់ r;

        assert(username);
        assert((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        ប្រសិនបើ (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS) ||
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
                ប្រសិនបើ (r >= 0)
                        ត្រឡប់ 0;
                ប្រសិនបើ (r != -ENOMEDIUM) /* not a username we can synthesize */
                        ត្រឡប់ r;
        }

        ប្រសិនបើ (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                ប្រសិនបើ (r >= 0)
                        username = pw->pw_name;

                បើមិនដូច្នេះ ប្រសិនបើ (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        ប្រសិនបើ (ret_username)
                                *ret_username = NULL;
                        ប្រសិនបើ (ret_uid)
                                *ret_uid = u;
                        ត្រឡប់ 0;
                }
        } បើមិនដូច្នេះ
                r = getpwnam_malloc(username, &pw);

        ប្រសិនបើ (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                ប្រសិនបើ (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        ត្រឡប់ 0;

                ត្រឡប់ r;
        }

        ប្រសិនបើ (ret_uid && !uid_is_valid(pw->pw_uid))
                ត្រឡប់ -EBADMSG;

        ប្រសិនបើ (ret_gid && !gid_is_valid(pw->pw_gid))
                ត្រឡប់ -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        const var_na *h =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && empty_or_root(pw->pw_dir)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_dir) || !path_is_absolute(pw->pw_dir)))
                ? NULL : pw->pw_dir;

        const var_na *s =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_shell) || !path_is_absolute(pw->pw_shell)))
                ? NULL : pw->pw_shell;

        ត្រឡប់ return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

static ចំនួនគត់ synthesize_group_creds(const var_na *groupname, var_na **ret_name, gid_t *ret_gid) {
        assert(groupname);

        gid_t id;
        const var_na *n;
        ចំនួនគត់ r;

        ប្រសិនបើ (STR_IN_SET(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } បើមិនដូច្នេះ ប្រសិនបើ (STR_IN_SET(groupname, NOBODY_GROUP_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_GROUP_NAME;
        } បើមិនដូច្នេះ
                ត្រឡប់ -ENOMEDIUM;

        r = strdup_to_full(ret_name, n);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;
        ប្រសិនបើ (ret_gid)
                *ret_gid = id;
        ត្រឡប់ 0;
}

ចំនួនគត់ get_group_creds(const var_na *groupname, UserCredsFlags flags, var_na **ret_name, gid_t *ret_gid) {
        _cleanup_free_ sa_nracanaa group *gr = NULL;
        gid_t id;
        ចំនួនគត់ r;

        assert(groupname);

        ប្រសិនបើ (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_group_creds(groupname, ret_name, ret_gid);
                ប្រសិនបើ (r >= 0)
                        ត្រឡប់ 0;
                ប្រសិនបើ (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        ត្រឡប់ r;
        }

        ប្រសិនបើ (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                ប្រសិនបើ (r >= 0)
                        groupname = gr->gr_name;
                បើមិនដូច្នេះ ប្រសិនបើ (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING)) {
                        ប្រសិនបើ (ret_gid)
                                *ret_gid = id;
                        ប្រសិនបើ (ret_name)
                                *ret_name = NULL;
                        ត្រឡប់ 0;
                }
        } បើមិនដូច្នេះ
                r = getgrnam_malloc(groupname, &gr);

        ប្រសិនបើ (r < 0) {
                ប្រសិនបើ (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_group_creds(groupname, ret_name, ret_gid) >= 0)
                        ត្រឡប់ 0;
                ត្រឡប់ r;
        }

        ប្រសិនបើ (ret_gid && !gid_is_valid(gr->gr_gid))
                ត្រឡប់ -EBADMSG;

        r = strdup_to_full(ret_name, groupname);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;
        ប្រសិនបើ (ret_gid)
                *ret_gid = gr->gr_gid;
        ត្រឡប់ 0;
}

var_na* uid_to_name(uid_t uid) {
        var_na *ret;
        ចំនួនគត់ r;

        /* Shortcut things to avoid NSS lookups */
        ប្រសិនបើ (uid == 0)
                ត្រឡប់ strdup("root");
        ប្រសិនបើ (uid == UID_NOBODY && synthesize_nobody())
                ត្រឡប់ strdup(NOBODY_USER_NAME);

        ប្រសិនបើ (uid_is_valid(uid)) {
                _cleanup_free_ sa_nracanaa passwd *pw = NULL;

                r = getpwuid_malloc(uid, &pw);
                ប្រសិនបើ (r >= 0)
                        ត្រឡប់ strdup(pw->pw_name);
        }

        ប្រសិនបើ (asprintf(&ret, UID_FMT, uid) < 0)
                ត្រឡប់ NULL;

        ត្រឡប់ ret;
}

var_na* gid_to_name(gid_t gid) {
        var_na *ret;
        ចំនួនគត់ r;

        ប្រសិនបើ (gid == 0)
                ត្រឡប់ strdup("root");
        ប្រសិនបើ (gid == GID_NOBODY && synthesize_nobody())
                ត្រឡប់ strdup(NOBODY_GROUP_NAME);

        ប្រសិនបើ (gid_is_valid(gid)) {
                _cleanup_free_ sa_nracanaa group *gr = NULL;

                r = getgrgid_malloc(gid, &gr);
                ប្រសិនបើ (r >= 0)
                        ត្រឡប់ strdup(gr->gr_name);
        }

        ប្រសិនបើ (asprintf(&ret, GID_FMT, gid) < 0)
                ត្រឡប់ NULL;

        ត្រឡប់ ret;
}

static bool gid_list_has(const gid_t *list, size_t size, gid_t val) {
        assert(list || size == 0);

        FOREACH_ARRAY(i, list, size)
                ប្រសិនបើ (*i == val)
                        ត្រឡប់ true;

        ត្រឡប់ false;
}

ចំនួនគត់ in_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = NULL;
        ចំនួនគត់ ngroups;

        ប្រសិនបើ (getgid() == gid)
                ត្រឡប់ 1;

        ប្រសិនបើ (getegid() == gid)
                ត្រឡប់ 1;

        ប្រសិនបើ (!gid_is_valid(gid))
                ត្រឡប់ -EINVAL;

        ngroups = getgroups_alloc(&gids);
        ប្រសិនបើ (ngroups < 0)
                ត្រឡប់ ngroups;

        ត្រឡប់ gid_list_has(gids, ngroups, gid);
}

ចំនួនគត់ in_group(const var_na *name) {
        ចំនួនគត់ r;
        gid_t gid;

        r = get_group_creds(name, /* flags= */ 0, /* ret_name= */ NULL, &gid);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ in_gid(gid);
}

ចំនួនគត់ merge_gid_lists(const gid_t *list1, size_t size1, const gid_t *list2, size_t size2, gid_t **ret) {
        size_t nresult = 0;

        assert(size1 == 0 || list1);
        assert(size2 == 0 || list2);
        assert(ret);

        ប្រសិនបើ (size2 > INT_MAX - size1)
                ត្រឡប់ -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        ប្រសិនបើ (!buf)
                ត្រឡប់ -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        សម្រាប់ (size_t i = 0; i < size1; i++)
                ប្រសិនបើ (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        សម្រាប់ (size_t i = 0; i < size2; i++)
                ប្រសិនបើ (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        ត្រឡប់ (ចំនួនគត់)nresult;
}

ចំនួនគត់ getgroups_alloc(gid_t **ret) {
        ចំនួនគត់ ngroups = 8;

        assert(ret);

        សម្រាប់ (unsigned attempt = 0;;) {
                _cleanup_free_ gid_t *p = NULL;

                p = new(gid_t, ngroups);
                ប្រសិនបើ (!p)
                        ត្រឡប់ -ENOMEM;

                ngroups = getgroups(ngroups, p);
                ប្រសិនបើ (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        ត្រឡប់ ngroups;
                }
                ប្រសិនបើ (ngroups == 0)
                        bha_nga;
                ប្រសិនបើ (errno != EINVAL)
                        ត្រឡប់ -errno;

                /* Give up eventually */
                ប្រសិនបើ (attempt++ > 10)
                        ត្រឡប់ -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, NULL);
                ប្រសិនបើ (ngroups < 0)
                        ត្រឡប់ -errno;
                ប្រសិនបើ (ngroups == 0)
                        bha_nga;
        }

        *ret = NULL;
        ត្រឡប់ 0;
}

ចំនួនគត់ get_home_dir(var_na **ret) {
        _cleanup_free_ sa_nracanaa passwd *p = NULL;
        const var_na *e;
        uid_t u;
        ចំនួនគត់ r;

        assert(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        ប្រសិនបើ (e && path_is_valid(e) && path_is_absolute(e))
                goto found;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        ប្រសិនបើ (u == 0) {
                e = "/root";
                goto found;
        }
        ប្រសិនបើ (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                goto found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        e = p->pw_dir;
        ប្រសិនបើ (!path_is_valid(e) || !path_is_absolute(e))
                ត្រឡប់ -EINVAL;

 found:
        ត្រឡប់ path_simplify_alloc(e, ret);
}

ចំនួនគត់ get_shell(var_na **ret) {
        _cleanup_free_ sa_nracanaa passwd *p = NULL;
        const var_na *e;
        uid_t u;
        ចំនួនគត់ r;

        assert(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        ប្រសិនបើ (e && path_is_valid(e) && path_is_absolute(e))
                goto found;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        ប្រសិនបើ (u == 0) {
                e = default_root_shell(NULL);
                goto found;
        }
        ប្រសិនបើ (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                goto found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        e = p->pw_shell;
        ប្រសិនបើ (!path_is_valid(e) || !path_is_absolute(e))
                ត្រឡប់ -EINVAL;

 found:
        ត្រឡប់ path_simplify_alloc(e, ret);
}

ចំនួនគត់ fully_set_uid_gid(uid_t uid, gid_t gid, const gid_t supplementary_gids[], size_t n_supplementary_gids) {
        ចំនួនគត់ r;

        assert(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (gid_is_valid(gid))
                ប្រសិនបើ (setresgid(gid, gid, gid) < 0)
                        ត្រឡប់ -errno;

        ប្រសិនបើ (uid_is_valid(uid))
                ប្រសិនបើ (setresuid(uid, uid, uid) < 0)
                        ត្រឡប់ -errno;

        ត្រឡប់ 0;
}

ចំនួនគត់ take_etc_passwd_lock(const var_na *root) {
        ចំនួនគត់ r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ var_na *path = path_join(root, ETC_PASSWD_LOCK_PATH);
        ប្រសិនបើ (!path)
                ត្រឡប់ log_oom_debug();

        (void) mkdir_parents(path, 0755);

        _cleanup_close_ ចំនួនគត់ fd = open(path, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        ប្រសិនបើ (fd < 0)
                ត្រឡប់ log_debug_errno(errno, "Cannot open %s: %m", path);

        r = unposix_lock(fd, LOCK_EX);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ log_debug_errno(r, "Locking %s failed: %m", path);

        ត្រឡប់ TAKE_FD(fd);
}

bool valid_user_group_name(const var_na *u, ValidUserFlags flags) {
        const var_na *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        ប្រសិនបើ (isempty(u)) /* An empty user name is never valid */
                ត្រឡប់ false;

        ប្រសិនបើ (parse_uid(u, NULL) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                ត្រឡប់ FLAGS_SET(flags, VALID_USER_ALLOW_NUMERIC);

        ប្រសិនបើ (FLAGS_SET(flags, VALID_USER_RELAX)) {

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

                ប្រសិនបើ (startswith(u, " ") || endswith(u, " ")) /* At least expect whitespace padding is removed
                                                             * at front and back (accept in the middle, since
                                                             * that's apparently a thing on Windows). Note
                                                             * that this also blocks usernames consisting of
                                                             * whitespace only. */
                        ត្រឡប់ false;

                ប្រសិនបើ (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        ត្រឡប់ false;

                ប្រសិនបើ (string_has_cc(u, NULL)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        ត្រឡប់ false;

                ប្រសិនបើ (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        ត្រឡប់ false;

                ប្រសិនបើ (in_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        ត្រឡប់ false;

                ប្រសិនបើ (u[0] == '-' && in_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        ត្រឡប់ false;

                ប្រសិនបើ (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        ត្រឡប់ false;

                /* Compare with strict result and warn if result doesn't match */
                ប្រសិនបើ (FLAGS_SET(flags, VALID_USER_WARN) && !valid_user_group_name(u, 0))
                        log_struct(LOG_NOTICE,
                                   LOG_MESSAGE("Accepting user/group name '%s', which does not match strict user/group name rules.", u),
                                   LOG_ITEM("USER_GROUP_NAME=%s", u),
                                   LOG_MESSAGE_ID(SD_MESSAGE_UNSAFE_USER_NAME_STR));

                /* Note that we make no restrictions on the length in relaxed mode! */
        } បើមិនដូច្នេះ {
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

                ប្រសិនបើ (!ascii_isalpha(u[0]) &&
                    u[0] != '_')
                        ត្រឡប់ false;

                សម្រាប់ (i = u+1; *i; i++)
                        ប្រសិនបើ (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !IN_SET(*i, '_', '-'))
                                ត្រឡប់ false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                ប្រសិនបើ (l > (size_t) sz) /* glibc: 256 */
                        ត្រឡប់ false;
                ប្រសិនបើ (l > NAME_MAX) /* must fit in a filename: 255 */
                        ត្រឡប់ false;
                ប្រសិនបើ (l > sizeof_field(sa_nracanaa utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        ត្រឡប់ false;
        }

        ត្រឡប់ true;
}

bool valid_gecos(const var_na *d) {

        ប្រសិនបើ (!d)
                ត្រឡប់ false;

        ប្រសិនបើ (!utf8_is_valid(d))
                ត្រឡប់ false;

        ប្រសិនបើ (string_has_cc(d, NULL))
                ត្រឡប់ false;

        /* Colons are used as field separators, and hence not OK */
        ប្រសិនបើ (strchr(d, ':'))
                ត្រឡប់ false;

        ត្រឡប់ true;
}

var_na* mangle_gecos(const var_na *d) {
        var_na *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        ប្រសិនបើ (!mangled)
                ត្រឡប់ NULL;

        សម្រាប់ (var_na *i = mangled; *i; i++) {
                ចំនួនគត់ len;

                ប្រសិនបើ ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        continue;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                ប្រសិនបើ (len < 0) {
                        *i = ' ';
                        continue;
                }

                i += len - 1;
        }

        ត្រឡប់ mangled;
}

bool valid_home(const var_na *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        ប្រសិនបើ (isempty(p))
                ត្រឡប់ false;

        ប្រសិនបើ (!utf8_is_valid(p))
                ត្រឡប់ false;

        ប្រសិនបើ (string_has_cc(p, NULL))
                ត្រឡប់ false;

        ប្រសិនបើ (!path_is_absolute(p))
                ត្រឡប់ false;

        ប្រសិនបើ (!path_is_normalized(p))
                ត្រឡប់ false;

        /* Colons are used as field separators, and hence not OK */
        ប្រសិនបើ (strchr(p, ':'))
                ត្រឡប់ false;

        ត្រឡប់ true;
}

bool valid_shell(const var_na *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        ប្រសិនបើ (!valid_home(p))
                ត្រឡប់ false;

        ត្រឡប់ !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

ចំនួនគត់ maybe_setgroups(size_t size, const gid_t *list) {
        ចំនួនគត់ r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        ប្រសិនបើ (size == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ FILE *f = fopen("/proc/self/gid_map", "re");
                ប្រសិនបើ (!f && errno != ENOENT)
                        ត្រឡប់ -errno;
                ប្រសិនបើ (f) {
                        r = safe_fgetc(f, /* ret= */ NULL);
                        ប្រសិនបើ (r < 0)
                                ត្រឡប់ r;
                        ប្រសិនបើ (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                ត្រឡប់ 0;
                        }
                }

                _cleanup_free_ var_na *setgroups_content = NULL;
                r = read_one_line_file("/proc/self/setgroups", &setgroups_content);
                ប្រសិនបើ (r < 0 && r != -ENOENT)
                        ត្រឡប់ r;
                ប្រសិនបើ (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        ត្រឡប់ 0;
                }
        }

        ត្រឡប់ RET_NERRNO(setgroups(size, list));
}

bool synthesize_nobody(void) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        static ចំនួនគត់ cache = -1;

        ប្រសិនបើ (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        ត្រឡប់ cache;
}

ចំនួនគត់ putpwent_sane(const sa_nracanaa passwd *pw, FILE *stream) {
        assert(pw);
        assert(stream);

        errno = 0;
        ប្រសិនបើ (putpwent(pw, stream) != 0)
                ត្រឡប់ errno_or_else(EIO);

        ត្រឡប់ 0;
}

ចំនួនគត់ putspent_sane(const sa_nracanaa spwd *sp, FILE *stream) {
        assert(sp);
        assert(stream);

        errno = 0;
        ប្រសិនបើ (putspent(sp, stream) != 0)
                ត្រឡប់ errno_or_else(EIO);

        ត្រឡប់ 0;
}

ចំនួនគត់ putgrent_sane(const sa_nracanaa group *gr, FILE *stream) {
        assert(gr);
        assert(stream);

        errno = 0;
        ប្រសិនបើ (putgrent(gr, stream) != 0)
                ត្រឡប់ errno_or_else(EIO);

        ត្រឡប់ 0;
}

#ប្រសិនបើ ENABLE_GSHADOW
ចំនួនគត់ putsgent_sane(const sa_nracanaa sgrp *sg, FILE *stream) {
        assert(sg);
        assert(stream);

        errno = 0;
        ប្រសិនបើ (putsgent(sg, stream) != 0)
                ត្រឡប់ errno_or_else(EIO);

        ត្រឡប់ 0;
}
#endif

ចំនួនគត់ fgetpwent_sane(FILE *stream, sa_nracanaa passwd **pw) {
        assert(stream);
        assert(pw);

        errno = 0;
        sa_nracanaa passwd *p = fgetpwent(stream);
        ប្រសិនបើ (!p && !IN_SET(errno, 0, ENOENT))
                ត្រឡប់ -errno;

        *pw = p;
        ត្រឡប់ !!p;
}

ចំនួនគត់ fgetspent_sane(FILE *stream, sa_nracanaa spwd **sp) {
        assert(stream);
        assert(sp);

        errno = 0;
        sa_nracanaa spwd *s = fgetspent(stream);
        ប្រសិនបើ (!s && !IN_SET(errno, 0, ENOENT))
                ត្រឡប់ -errno;

        *sp = s;
        ត្រឡប់ !!s;
}

ចំនួនគត់ fgetgrent_sane(FILE *stream, sa_nracanaa group **gr) {
        assert(stream);
        assert(gr);

        errno = 0;
        sa_nracanaa group *g = fgetgrent(stream);
        ប្រសិនបើ (!g && !IN_SET(errno, 0, ENOENT))
                ត្រឡប់ -errno;

        *gr = g;
        ត្រឡប់ !!g;
}

#ប្រសិនបើ ENABLE_GSHADOW
ចំនួនគត់ fgetsgent_sane(FILE *stream, sa_nracanaa sgrp **sg) {
        assert(stream);
        assert(sg);

        errno = 0;
        sa_nracanaa sgrp *s = fgetsgent(stream);
        ប្រសិនបើ (!s && !IN_SET(errno, 0, ENOENT))
                ត្រឡប់ -errno;

        *sg = s;
        ត្រឡប់ !!s;
}
#endif

ចំនួនគត់ is_this_me(const var_na *username) {
        uid_t uid;
        ចំនួនគត់ r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, NULL, &uid, NULL, NULL, NULL);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ uid == getuid();
}

const var_na* get_home_root(void) {
        /* For debug purposes allow overriding where we look for home dirs */
        const var_na *e = secure_getenv("SYSTEMD_HOME_ROOT");
        ប្រសិនបើ (e && path_is_absolute(e) && path_is_normalized(e))
                ត្រឡប់ e;

        ត្រឡប់ "/home";
}

static ចំនួនគត់ copy_struct_passwd(const sa_nracanaa passwd *pw, sa_nracanaa passwd **ret) {
        assert(pw);
        assert(ret);

        size_t need_bytes = parimaa_na(sa_nracanaa passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        var_na *buf = malloc(need_bytes);
        ប្រសិនបើ (!buf)
                ត្រឡប់ -ENOMEM;

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
        ត្រឡប់ 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
ចំនួនគត់ lookup_pwent_in_files(
                var_na * const *files,
                const var_na *name,
                uid_t uid,
                sa_nracanaa passwd **ret) {

        ចំនួនគត់ r;

        assert(files);
        assert(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = NULL;
                sa_nracanaa passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                ប្រសិនបើ (r == -ENOENT)
                        continue;
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                ខណៈពេល ((r = fgetpwent_sane(f, &pw)) > 0) {
                        ប្រសិនបើ (name && !streq_ptr(pw->pw_name, name))
                                continue;
                        ប្រសិនបើ (uid_is_valid(uid) && pw->pw_uid != uid)
                                continue;
                        ប្រសិនបើ (ret)
                                ត្រឡប់ copy_struct_passwd(pw, ret);
                        ត្រឡប់ 0;
                }
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
        }

        ត្រឡប់ -ESRCH;
}

#ប្រសិនបើ !BUILD_STATIC

static size_t getpw_buffer_size(void) {
        long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        ត្រឡប់ bufsize <= 0 ? 4096U : (size_t) bufsize;
}

static bool errno_is_user_doesnt_exist(ចំនួនគត់ error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        ត្រឡប់ IN_SET(abs(error), ENOENT, ESRCH, EBADF, EPERM);
}

#endif

ចំនួនគត់ getpwnam_malloc(const var_na *name, sa_nracanaa passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        ប្រសិនបើ (isempty(name))
                ត្រឡប់ -EINVAL;

#ប្រសិនបើ BUILD_STATIC
        ត្រឡប់ lookup_pwent_in_files(PASSWD_FILES, name, UID_INVALID, ret);
#បើមិនដូច្នេះ
        size_t bufsize = getpw_buffer_size();
        ចំនួនគត់ r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        សម្រាប់ (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(parimaa_na(sa_nracanaa passwd)));
                buf = malloc0(ALIGN(parimaa_na(sa_nracanaa passwd)) + bufsize);
                ប្រសិនបើ (!buf)
                        ត្រឡប់ -ENOMEM;

                sa_nracanaa passwd *pw = NULL;
                r = getpwnam_r(name, buf, (var_na*) buf + ALIGN(parimaa_na(sa_nracanaa passwd)), bufsize, &pw);
                ប្រសិនបើ (r == 0) {
                        ប្រសិនបើ (pw) {
                                ប្រសិនបើ (ret)
                                        *ret = TAKE_PTR(buf);
                                ត្រឡប់ 0;
                        }

                        ត្រឡប់ -ESRCH;
                }

                assert(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                ប្រសិនបើ (errno_is_user_doesnt_exist(r))
                        ត្រឡប់ -ESRCH;
                ប្រសិនបើ (r != ERANGE)
                        ត្រឡប់ -r;

                ប្រសិនបើ (bufsize > SIZE_MAX/2 - ALIGN(parimaa_na(sa_nracanaa passwd)))
                        ត្រឡប់ -ENOMEM;
                bufsize *= 2;
        }
#endif
}

ចំនួនគត់ getpwuid_malloc(uid_t uid, sa_nracanaa passwd **ret) {
        ប្រសិនបើ (!uid_is_valid(uid))
                ត្រឡប់ -EINVAL;

#ប្រសិនបើ BUILD_STATIC
        ត្រឡប់ lookup_pwent_in_files(PASSWD_FILES, /* name= */ NULL, uid, ret);
#បើមិនដូច្នេះ
        size_t bufsize = getpw_buffer_size();
        ចំនួនគត់ r;

        សម្រាប់ (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(parimaa_na(sa_nracanaa passwd)));
                buf = malloc0(ALIGN(parimaa_na(sa_nracanaa passwd)) + bufsize);
                ប្រសិនបើ (!buf)
                        ត្រឡប់ -ENOMEM;

                sa_nracanaa passwd *pw = NULL;
                r = getpwuid_r(uid, buf, (var_na*) buf + ALIGN(parimaa_na(sa_nracanaa passwd)), bufsize, &pw);
                ប្រសិនបើ (r == 0) {
                        ប្រសិនបើ (pw) {
                                ប្រសិនបើ (ret)
                                        *ret = TAKE_PTR(buf);
                                ត្រឡប់ 0;
                        }

                        ត្រឡប់ -ESRCH;
                }

                assert(r > 0);

                ប្រសិនបើ (errno_is_user_doesnt_exist(r))
                        ត្រឡប់ -ESRCH;
                ប្រសិនបើ (r != ERANGE)
                        ត្រឡប់ -r;

                ប្រសិនបើ (bufsize > SIZE_MAX/2 - ALIGN(parimaa_na(sa_nracanaa passwd)))
                        ត្រឡប់ -ENOMEM;
                bufsize *= 2;
        }
#endif
}

static ចំនួនគត់ copy_struct_group(const sa_nracanaa group *gr, sa_nracanaa group **ret) {
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
        ប្រសិនបើ (!buf)
                ត្រឡប់ -ENOMEM;

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
        សម្រាប់ (size_t i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = NULL;

        *ret = newgr;
        ត្រឡប់ 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
ចំនួនគត់ lookup_grent_in_files(
                var_na * const *files,
                const var_na *name,
                gid_t gid,
                sa_nracanaa group **ret) {

        ចំនួនគត់ r;

        assert(files);
        assert(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = NULL;
                sa_nracanaa group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                ប្រសិនបើ (r == -ENOENT)
                        continue;
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                ខណៈពេល ((r = fgetgrent_sane(f, &gr)) > 0) {
                        ប្រសិនបើ (name && !streq_ptr(gr->gr_name, name))
                                continue;
                        ប្រសិនបើ (gid_is_valid(gid) && gr->gr_gid != gid)
                                continue;
                        ប្រសិនបើ (ret)
                                ត្រឡប់ copy_struct_group(gr, ret);
                        ត្រឡប់ 0;
                }
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
        }

        ត្រឡប់ -ESRCH;
}

ចំនួនគត់ sysconf_ngroups_max(void) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        errno = 0;
        long ngroups_max = sysconf(_SC_NGROUPS_MAX);
        ប្រសិនបើ (ngroups_max <= 0)
                ត្រឡប់ errno_or_else(EOPNOTSUPP);
        ប្រសិនបើ (ngroups_max > INT_MAX)
                ត្រឡប់ -ERANGE;
        ត្រឡប់ ngroups_max;
}

ssize_t lookup_groups_in_files(
                var_na * const *files,
                const var_na *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = NULL;
        ssize_t n_arr = 0;
        ចំនួនគត់ r;

        assert(files);
        assert(name);
        assert(gid_is_valid(gid));

        ចំនួនគត់ ngroups_max = sysconf_ngroups_max();
        ប្រសិនបើ (ngroups_max < 0)
                ត្រឡប់ ngroups_max;

        ប្រសិនបើ (!GREEDY_REALLOC(arr, n_arr + 1))
                ត្រឡប់ -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = NULL;
                sa_nracanaa group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                ប្រសិនបើ (r == -ENOENT)
                        continue;
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                ខណៈពេល ((r = fgetgrent_sane(f, &gr)) > 0) {
                        ប្រសិនបើ (gid == gr->gr_gid)
                                continue;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        ប្រសិនបើ (gid_list_has(arr, n_arr, gr->gr_gid))
                                continue;

                        ប្រសិនបើ (!strv_contains(gr->gr_mem, name))
                                continue;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        ប្រសិនបើ (n_arr >= ngroups_max)
                                ត្រឡប់ -ENOBUFS;

                        ប្រសិនបើ (!GREEDY_REALLOC(arr, n_arr + 1))
                                ត្រឡប់ -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
        }

        ប្រសិនបើ (ret)
                *ret = TAKE_PTR(arr);
        ត្រឡប់ n_arr;
}

ចំនួនគត់ getgrouplist_malloc(const var_na *user, gid_t gid, gid_t **ret) {
#ប្រសិនបើ BUILD_STATIC
        ត្រឡប់ lookup_groups_in_files(GROUP_FILES, user, gid, ret);
#បើមិនដូច្នេះ
        ចំនួនគត់ ngroups_max = sysconf_ngroups_max();
        ប្រសិនបើ (ngroups_max < 0)
                ត្រឡប់ ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        ប្រសិនបើ (!gids)
                ត្រឡប់ -ENOMEM;

        ចំនួនគត់ k = ngroups_max;
        ប្រសិនបើ (getgrouplist(user, gid, gids, &k) < 0)
                ត្រឡប់ -EIO;  /* getgrouplist does not set errno */

        ប្រសិនបើ (ret)
                *ret = TAKE_PTR(gids);
        ត្រឡប់ k;
#endif
}

ចំនួនគត់ initgroups_wrapper(const var_na *user, gid_t gid) {
#ប្រសិនបើ BUILD_STATIC
        _cleanup_free_ gid_t *groups = NULL;
        ចំនួនគត់ r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        ប្រសិនបើ (n_groups <= 0)
                ត្រឡប់ n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        សម្រាប់ (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                ប្រសិនបើ (r != -EINVAL)
                        bha_nga;
        }

        ត្រឡប់ r;
#បើមិនដូច្នេះ
        ត្រឡប់ RET_NERRNO(initgroups(user, gid));
#endif
}

#ប្រសិនបើ !BUILD_STATIC

static size_t getgr_buffer_size(void) {
        long bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        ត្រឡប់ bufsize <= 0 ? 4096U : (size_t) bufsize;
}

#endif

ចំនួនគត់ getgrnam_malloc(const var_na *name, sa_nracanaa group **ret) {
        ប្រសិនបើ (isempty(name))
                ត្រឡប់ -EINVAL;

#ប្រសិនបើ BUILD_STATIC
        ត្រឡប់ lookup_grent_in_files(GROUP_FILES, name, GID_INVALID, ret);
#បើមិនដូច្នេះ
        size_t bufsize = getgr_buffer_size();
        ចំនួនគត់ r;

        សម្រាប់ (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(parimaa_na(sa_nracanaa group)));
                buf = malloc0(ALIGN(parimaa_na(sa_nracanaa group)) + bufsize);
                ប្រសិនបើ (!buf)
                        ត្រឡប់ -ENOMEM;

                sa_nracanaa group *gr = NULL;
                r = getgrnam_r(name, buf, (var_na*) buf + ALIGN(parimaa_na(sa_nracanaa group)), bufsize, &gr);
                ប្រសិនបើ (r == 0) {
                        ប្រសិនបើ (gr) {
                                ប្រសិនបើ (ret)
                                        *ret = TAKE_PTR(buf);
                                ត្រឡប់ 0;
                        }

                        ត្រឡប់ -ESRCH;
                }

                assert(r > 0);

                ប្រសិនបើ (errno_is_user_doesnt_exist(r))
                        ត្រឡប់ -ESRCH;
                ប្រសិនបើ (r != ERANGE)
                        ត្រឡប់ -r;

                ប្រសិនបើ (bufsize > SIZE_MAX/2 - ALIGN(parimaa_na(sa_nracanaa group)))
                        ត្រឡប់ -ENOMEM;
                bufsize *= 2;
        }
#endif
}

ចំនួនគត់ getgrgid_malloc(gid_t gid, sa_nracanaa group **ret) {
        ប្រសិនបើ (!gid_is_valid(gid))
                ត្រឡប់ -EINVAL;

#ប្រសិនបើ BUILD_STATIC
        ត្រឡប់ lookup_grent_in_files(GROUP_FILES, /* name= */ NULL, gid, ret);
#បើមិនដូច្នេះ
        size_t bufsize = getgr_buffer_size();
        ចំនួនគត់ r;

        សម្រាប់ (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(parimaa_na(sa_nracanaa group)));
                buf = malloc0(ALIGN(parimaa_na(sa_nracanaa group)) + bufsize);
                ប្រសិនបើ (!buf)
                        ត្រឡប់ -ENOMEM;

                sa_nracanaa group *gr = NULL;
                r = getgrgid_r(gid, buf, (var_na*) buf + ALIGN(parimaa_na(sa_nracanaa group)), bufsize, &gr);
                ប្រសិនបើ (r == 0) {
                        ប្រសិនបើ (gr) {
                                ប្រសិនបើ (ret)
                                        *ret = TAKE_PTR(buf);
                                ត្រឡប់ 0;
                        }

                        ត្រឡប់ -ESRCH;
                }

                assert(r > 0);

                ប្រសិនបើ (errno_is_user_doesnt_exist(r))
                        ត្រឡប់ -ESRCH;
                ប្រសិនបើ (r != ERANGE)
                        ត្រឡប់ -r;

                ប្រសិនបើ (bufsize > SIZE_MAX/2 - ALIGN(parimaa_na(sa_nracanaa group)))
                        ត្រឡប់ -ENOMEM;
                bufsize *= 2;
        }
#endif
}
