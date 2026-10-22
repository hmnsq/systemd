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
                        ကိန်းပြည့် errnum,                                     \
                        var_na *buf,                                      \
                        size_t buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                switch (errnum) {                                       \
                pak_sa ESRCH:                                             \
                        ပြန်ပေး "Unknown " STRINGIFY(type);              \
                pak_sa ENOEXEC:                                           \
                        ပြန်ပေး "Not a system " STRINGIFY(type);         \
                default:                                                \
                        ပြန်ပေး strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        အကယ်၍ (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                ပြန်ပေး false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        အကယ်၍ (uid == (uid_t) UINT32_C(0xFFFF))
                ပြန်ပေး false;

        ပြန်ပေး true;
}

ကိန်းပြည့် parse_uid(const var_na *s, uid_t *ret) {
        uint32_t uid = 0;
        ကိန်းပြည့် r;

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
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (!uid_is_valid(uid))
                ပြန်ပေး -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        အကယ်၍ (ret)
                *ret = uid;

        ပြန်ပေး 0;
}

ကိန်းပြည့် parse_uid_range(const var_na *s, uid_t *ret_lower, uid_t *ret_upper) {
        _cleanup_free_ var_na *word = NULL;
        uid_t l, u;
        ကိန်းပြည့် r;

        assert(s);
        assert(ret_lower);
        assert(ret_upper);

        r = extract_first_word(&s, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        အကယ်၍ (r == 0)
                ပြန်ပေး -EINVAL;

        r = parse_uid(word, &l);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        /* Check for the upper bound and extract it if needed */
        အကယ်၍ (!s)
                /* Single number with no dash. */
                u = l;
        မဟုတ်ပါက အကယ်၍ (!*s)
                /* Trailing dash is an error. */
                ပြန်ပေး -EINVAL;
        မဟုတ်ပါက {
                r = parse_uid(s, &u);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                အကယ်၍ (l > u)
                        ပြန်ပေး -EINVAL;
        }

        *ret_lower = l;
        *ret_upper = u;
        ပြန်ပေး 0;
}

var_na* getlogname_malloc(void) {
        uid_t uid;
        sa_nracanaa stat st;

        အကယ်၍ (isatty_safe(STDIN_FILENO) && fstat(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        မဟုတ်ပါက
                uid = getuid();

        ပြန်ပေး uid_to_name(uid);
}

var_na* getusername_malloc(void) {
        const var_na *e;

        e = secure_getenv("USER");
        အကယ်၍ (e)
                ပြန်ပေး strdup(e);

        ပြန်ပေး uid_to_name(getuid());
}

bool is_nologin_shell(const var_na *shell) {
        ပြန်ပေး PATH_IN_SET(shell,
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
        ပြန်ပေး isempty(shell) || is_nologin_shell(shell);
}

const var_na* default_root_shell_at(ကိန်းပြည့် rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        assert(rfd >= 0 || rfd == AT_FDCWD);

        ကိန်းပြည့် r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, NULL, NULL);
        အကယ်၍ (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        အကယ်၍ (r > 0)
                ပြန်ပေး DEFAULT_USER_SHELL;

        ပြန်ပေး "/bin/sh";
}

const var_na* default_root_shell(const var_na *root) {
        _cleanup_close_ ကိန်းပြည့် rfd = -EBADF;

        rfd = open(empty_to_root(root), O_CLOEXEC | O_DIRECTORY | O_PATH);
        အကယ်၍ (rfd < 0)
                ပြန်ပေး "/bin/sh";

        ပြန်ပေး default_root_shell_at(rfd);
}

static ကိန်းပြည့် return_user_creds(
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
        ကိန်းပြည့် r;

        အကယ်၍ (ret_username) {
                r = strdup_to(&s1, username);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        အကယ်၍ (ret_home) {
                r = strdup_to(&s2, home);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        အကယ်၍ (ret_shell) {
                r = strdup_to(&s3, shell);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        အကယ်၍ (ret_username)
                *ret_username = TAKE_PTR(s1);
        အကယ်၍ (ret_uid)
                *ret_uid = uid;
        အကယ်၍ (ret_gid)
                *ret_gid = gid;
        အကယ်၍ (ret_home)
                *ret_home = TAKE_PTR(s2);
        အကယ်၍ (ret_shell)
                *ret_shell = TAKE_PTR(s3);
        ပြန်ပေး 0;
}

static ကိန်းပြည့် synthesize_user_creds(
                const var_na *username,
                UserCredsFlags flags,
                var_na **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                var_na **ret_home,
                var_na **ret_shell) {
        assert(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        အကယ်၍ (STR_IN_SET(username, "root", "0"))
                ပြန်ပေး return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_root_shell(NULL) : NULL,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        အကယ်၍ (STR_IN_SET(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                ပြန်ပေး return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? NULL : "/",
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? NULL : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        ပြန်ပေး -ENOMEDIUM;
}

ကိန်းပြည့် get_user_creds(
                const var_na *username,
                UserCredsFlags flags,
                var_na **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                var_na **ret_home,
                var_na **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ sa_nracanaa passwd *pw = NULL;
        ကိန်းပြည့် r;

        assert(username);
        assert((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        အကယ်၍ (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS) ||
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
                အကယ်၍ (r >= 0)
                        ပြန်ပေး 0;
                အကယ်၍ (r != -ENOMEDIUM) /* not a username we can synthesize */
                        ပြန်ပေး r;
        }

        အကယ်၍ (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                အကယ်၍ (r >= 0)
                        username = pw->pw_name;

                မဟုတ်ပါက အကယ်၍ (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        အကယ်၍ (ret_username)
                                *ret_username = NULL;
                        အကယ်၍ (ret_uid)
                                *ret_uid = u;
                        ပြန်ပေး 0;
                }
        } မဟုတ်ပါက
                r = getpwnam_malloc(username, &pw);

        အကယ်၍ (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                အကယ်၍ (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        ပြန်ပေး 0;

                ပြန်ပေး r;
        }

        အကယ်၍ (ret_uid && !uid_is_valid(pw->pw_uid))
                ပြန်ပေး -EBADMSG;

        အကယ်၍ (ret_gid && !gid_is_valid(pw->pw_gid))
                ပြန်ပေး -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        const var_na *h =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && empty_or_root(pw->pw_dir)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_dir) || !path_is_absolute(pw->pw_dir)))
                ? NULL : pw->pw_dir;

        const var_na *s =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_shell) || !path_is_absolute(pw->pw_shell)))
                ? NULL : pw->pw_shell;

        ပြန်ပေး return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

static ကိန်းပြည့် synthesize_group_creds(const var_na *groupname, var_na **ret_name, gid_t *ret_gid) {
        assert(groupname);

        gid_t id;
        const var_na *n;
        ကိန်းပြည့် r;

        အကယ်၍ (STR_IN_SET(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } မဟုတ်ပါက အကယ်၍ (STR_IN_SET(groupname, NOBODY_GROUP_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_GROUP_NAME;
        } မဟုတ်ပါက
                ပြန်ပေး -ENOMEDIUM;

        r = strdup_to_full(ret_name, n);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        အကယ်၍ (ret_gid)
                *ret_gid = id;
        ပြန်ပေး 0;
}

ကိန်းပြည့် get_group_creds(const var_na *groupname, UserCredsFlags flags, var_na **ret_name, gid_t *ret_gid) {
        _cleanup_free_ sa_nracanaa group *gr = NULL;
        gid_t id;
        ကိန်းပြည့် r;

        assert(groupname);

        အကယ်၍ (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_group_creds(groupname, ret_name, ret_gid);
                အကယ်၍ (r >= 0)
                        ပြန်ပေး 0;
                အကယ်၍ (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        ပြန်ပေး r;
        }

        အကယ်၍ (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                အကယ်၍ (r >= 0)
                        groupname = gr->gr_name;
                မဟုတ်ပါက အကယ်၍ (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING)) {
                        အကယ်၍ (ret_gid)
                                *ret_gid = id;
                        အကယ်၍ (ret_name)
                                *ret_name = NULL;
                        ပြန်ပေး 0;
                }
        } မဟုတ်ပါက
                r = getgrnam_malloc(groupname, &gr);

        အကယ်၍ (r < 0) {
                အကယ်၍ (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_group_creds(groupname, ret_name, ret_gid) >= 0)
                        ပြန်ပေး 0;
                ပြန်ပေး r;
        }

        အကယ်၍ (ret_gid && !gid_is_valid(gr->gr_gid))
                ပြန်ပေး -EBADMSG;

        r = strdup_to_full(ret_name, groupname);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        အကယ်၍ (ret_gid)
                *ret_gid = gr->gr_gid;
        ပြန်ပေး 0;
}

var_na* uid_to_name(uid_t uid) {
        var_na *ret;
        ကိန်းပြည့် r;

        /* Shortcut things to avoid NSS lookups */
        အကယ်၍ (uid == 0)
                ပြန်ပေး strdup("root");
        အကယ်၍ (uid == UID_NOBODY && synthesize_nobody())
                ပြန်ပေး strdup(NOBODY_USER_NAME);

        အကယ်၍ (uid_is_valid(uid)) {
                _cleanup_free_ sa_nracanaa passwd *pw = NULL;

                r = getpwuid_malloc(uid, &pw);
                အကယ်၍ (r >= 0)
                        ပြန်ပေး strdup(pw->pw_name);
        }

        အကယ်၍ (asprintf(&ret, UID_FMT, uid) < 0)
                ပြန်ပေး NULL;

        ပြန်ပေး ret;
}

var_na* gid_to_name(gid_t gid) {
        var_na *ret;
        ကိန်းပြည့် r;

        အကယ်၍ (gid == 0)
                ပြန်ပေး strdup("root");
        အကယ်၍ (gid == GID_NOBODY && synthesize_nobody())
                ပြန်ပေး strdup(NOBODY_GROUP_NAME);

        အကယ်၍ (gid_is_valid(gid)) {
                _cleanup_free_ sa_nracanaa group *gr = NULL;

                r = getgrgid_malloc(gid, &gr);
                အကယ်၍ (r >= 0)
                        ပြန်ပေး strdup(gr->gr_name);
        }

        အကယ်၍ (asprintf(&ret, GID_FMT, gid) < 0)
                ပြန်ပေး NULL;

        ပြန်ပေး ret;
}

static bool gid_list_has(const gid_t *list, size_t size, gid_t val) {
        assert(list || size == 0);

        FOREACH_ARRAY(i, list, size)
                အကယ်၍ (*i == val)
                        ပြန်ပေး true;

        ပြန်ပေး false;
}

ကိန်းပြည့် in_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = NULL;
        ကိန်းပြည့် ngroups;

        အကယ်၍ (getgid() == gid)
                ပြန်ပေး 1;

        အကယ်၍ (getegid() == gid)
                ပြန်ပေး 1;

        အကယ်၍ (!gid_is_valid(gid))
                ပြန်ပေး -EINVAL;

        ngroups = getgroups_alloc(&gids);
        အကယ်၍ (ngroups < 0)
                ပြန်ပေး ngroups;

        ပြန်ပေး gid_list_has(gids, ngroups, gid);
}

ကိန်းပြည့် in_group(const var_na *name) {
        ကိန်းပြည့် r;
        gid_t gid;

        r = get_group_creds(name, /* flags= */ 0, /* ret_name= */ NULL, &gid);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး in_gid(gid);
}

ကိန်းပြည့် merge_gid_lists(const gid_t *list1, size_t size1, const gid_t *list2, size_t size2, gid_t **ret) {
        size_t nresult = 0;

        assert(size1 == 0 || list1);
        assert(size2 == 0 || list2);
        assert(ret);

        အကယ်၍ (size2 > INT_MAX - size1)
                ပြန်ပေး -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        အကယ်၍ (!buf)
                ပြန်ပေး -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        အတွက် (size_t i = 0; i < size1; i++)
                အကယ်၍ (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        အတွက် (size_t i = 0; i < size2; i++)
                အကယ်၍ (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        ပြန်ပေး (ကိန်းပြည့်)nresult;
}

ကိန်းပြည့် getgroups_alloc(gid_t **ret) {
        ကိန်းပြည့် ngroups = 8;

        assert(ret);

        အတွက် (unsigned attempt = 0;;) {
                _cleanup_free_ gid_t *p = NULL;

                p = new(gid_t, ngroups);
                အကယ်၍ (!p)
                        ပြန်ပေး -ENOMEM;

                ngroups = getgroups(ngroups, p);
                အကယ်၍ (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        ပြန်ပေး ngroups;
                }
                အကယ်၍ (ngroups == 0)
                        bha_nga;
                အကယ်၍ (errno != EINVAL)
                        ပြန်ပေး -errno;

                /* Give up eventually */
                အကယ်၍ (attempt++ > 10)
                        ပြန်ပေး -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, NULL);
                အကယ်၍ (ngroups < 0)
                        ပြန်ပေး -errno;
                အကယ်၍ (ngroups == 0)
                        bha_nga;
        }

        *ret = NULL;
        ပြန်ပေး 0;
}

ကိန်းပြည့် get_home_dir(var_na **ret) {
        _cleanup_free_ sa_nracanaa passwd *p = NULL;
        const var_na *e;
        uid_t u;
        ကိန်းပြည့် r;

        assert(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        အကယ်၍ (e && path_is_valid(e) && path_is_absolute(e))
                goto found;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        အကယ်၍ (u == 0) {
                e = "/root";
                goto found;
        }
        အကယ်၍ (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                goto found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        e = p->pw_dir;
        အကယ်၍ (!path_is_valid(e) || !path_is_absolute(e))
                ပြန်ပေး -EINVAL;

 found:
        ပြန်ပေး path_simplify_alloc(e, ret);
}

ကိန်းပြည့် get_shell(var_na **ret) {
        _cleanup_free_ sa_nracanaa passwd *p = NULL;
        const var_na *e;
        uid_t u;
        ကိန်းပြည့် r;

        assert(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        အကယ်၍ (e && path_is_valid(e) && path_is_absolute(e))
                goto found;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        အကယ်၍ (u == 0) {
                e = default_root_shell(NULL);
                goto found;
        }
        အကယ်၍ (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                goto found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        e = p->pw_shell;
        အကယ်၍ (!path_is_valid(e) || !path_is_absolute(e))
                ပြန်ပေး -EINVAL;

 found:
        ပြန်ပေး path_simplify_alloc(e, ret);
}

ကိန်းပြည့် fully_set_uid_gid(uid_t uid, gid_t gid, const gid_t supplementary_gids[], size_t n_supplementary_gids) {
        ကိန်းပြည့် r;

        assert(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (gid_is_valid(gid))
                အကယ်၍ (setresgid(gid, gid, gid) < 0)
                        ပြန်ပေး -errno;

        အကယ်၍ (uid_is_valid(uid))
                အကယ်၍ (setresuid(uid, uid, uid) < 0)
                        ပြန်ပေး -errno;

        ပြန်ပေး 0;
}

ကိန်းပြည့် take_etc_passwd_lock(const var_na *root) {
        ကိန်းပြည့် r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ var_na *path = path_join(root, ETC_PASSWD_LOCK_PATH);
        အကယ်၍ (!path)
                ပြန်ပေး log_oom_debug();

        (void) mkdir_parents(path, 0755);

        _cleanup_close_ ကိန်းပြည့် fd = open(path, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        အကယ်၍ (fd < 0)
                ပြန်ပေး log_debug_errno(errno, "Cannot open %s: %m", path);

        r = unposix_lock(fd, LOCK_EX);
        အကယ်၍ (r < 0)
                ပြန်ပေး log_debug_errno(r, "Locking %s failed: %m", path);

        ပြန်ပေး TAKE_FD(fd);
}

bool valid_user_group_name(const var_na *u, ValidUserFlags flags) {
        const var_na *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        အကယ်၍ (isempty(u)) /* An empty user name is never valid */
                ပြန်ပေး false;

        အကယ်၍ (parse_uid(u, NULL) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                ပြန်ပေး FLAGS_SET(flags, VALID_USER_ALLOW_NUMERIC);

        အကယ်၍ (FLAGS_SET(flags, VALID_USER_RELAX)) {

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

                အကယ်၍ (startswith(u, " ") || endswith(u, " ")) /* At least expect whitespace padding is removed
                                                             * at front and back (accept in the middle, since
                                                             * that's apparently a thing on Windows). Note
                                                             * that this also blocks usernames consisting of
                                                             * whitespace only. */
                        ပြန်ပေး false;

                အကယ်၍ (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        ပြန်ပေး false;

                အကယ်၍ (string_has_cc(u, NULL)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        ပြန်ပေး false;

                အကယ်၍ (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        ပြန်ပေး false;

                အကယ်၍ (in_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        ပြန်ပေး false;

                အကယ်၍ (u[0] == '-' && in_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        ပြန်ပေး false;

                အကယ်၍ (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        ပြန်ပေး false;

                /* Compare with strict result and warn if result doesn't match */
                အကယ်၍ (FLAGS_SET(flags, VALID_USER_WARN) && !valid_user_group_name(u, 0))
                        log_struct(LOG_NOTICE,
                                   LOG_MESSAGE("Accepting user/group name '%s', which does not match strict user/group name rules.", u),
                                   LOG_ITEM("USER_GROUP_NAME=%s", u),
                                   LOG_MESSAGE_ID(SD_MESSAGE_UNSAFE_USER_NAME_STR));

                /* Note that we make no restrictions on the length in relaxed mode! */
        } မဟုတ်ပါက {
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

                အကယ်၍ (!ascii_isalpha(u[0]) &&
                    u[0] != '_')
                        ပြန်ပေး false;

                အတွက် (i = u+1; *i; i++)
                        အကယ်၍ (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !IN_SET(*i, '_', '-'))
                                ပြန်ပေး false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                အကယ်၍ (l > (size_t) sz) /* glibc: 256 */
                        ပြန်ပေး false;
                အကယ်၍ (l > NAME_MAX) /* must fit in a filename: 255 */
                        ပြန်ပေး false;
                အကယ်၍ (l > sizeof_field(sa_nracanaa utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        ပြန်ပေး false;
        }

        ပြန်ပေး true;
}

bool valid_gecos(const var_na *d) {

        အကယ်၍ (!d)
                ပြန်ပေး false;

        အကယ်၍ (!utf8_is_valid(d))
                ပြန်ပေး false;

        အကယ်၍ (string_has_cc(d, NULL))
                ပြန်ပေး false;

        /* Colons are used as field separators, and hence not OK */
        အကယ်၍ (strchr(d, ':'))
                ပြန်ပေး false;

        ပြန်ပေး true;
}

var_na* mangle_gecos(const var_na *d) {
        var_na *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        အကယ်၍ (!mangled)
                ပြန်ပေး NULL;

        အတွက် (var_na *i = mangled; *i; i++) {
                ကိန်းပြည့် len;

                အကယ်၍ ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        continue;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                အကယ်၍ (len < 0) {
                        *i = ' ';
                        continue;
                }

                i += len - 1;
        }

        ပြန်ပေး mangled;
}

bool valid_home(const var_na *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        အကယ်၍ (isempty(p))
                ပြန်ပေး false;

        အကယ်၍ (!utf8_is_valid(p))
                ပြန်ပေး false;

        အကယ်၍ (string_has_cc(p, NULL))
                ပြန်ပေး false;

        အကယ်၍ (!path_is_absolute(p))
                ပြန်ပေး false;

        အကယ်၍ (!path_is_normalized(p))
                ပြန်ပေး false;

        /* Colons are used as field separators, and hence not OK */
        အကယ်၍ (strchr(p, ':'))
                ပြန်ပေး false;

        ပြန်ပေး true;
}

bool valid_shell(const var_na *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        အကယ်၍ (!valid_home(p))
                ပြန်ပေး false;

        ပြန်ပေး !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

ကိန်းပြည့် maybe_setgroups(size_t size, const gid_t *list) {
        ကိန်းပြည့် r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        အကယ်၍ (size == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ FILE *f = fopen("/proc/self/gid_map", "re");
                အကယ်၍ (!f && errno != ENOENT)
                        ပြန်ပေး -errno;
                အကယ်၍ (f) {
                        r = safe_fgetc(f, /* ret= */ NULL);
                        အကယ်၍ (r < 0)
                                ပြန်ပေး r;
                        အကယ်၍ (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                ပြန်ပေး 0;
                        }
                }

                _cleanup_free_ var_na *setgroups_content = NULL;
                r = read_one_line_file("/proc/self/setgroups", &setgroups_content);
                အကယ်၍ (r < 0 && r != -ENOENT)
                        ပြန်ပေး r;
                အကယ်၍ (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        ပြန်ပေး 0;
                }
        }

        ပြန်ပေး RET_NERRNO(setgroups(size, list));
}

bool synthesize_nobody(void) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        static ကိန်းပြည့် cache = -1;

        အကယ်၍ (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        ပြန်ပေး cache;
}

ကိန်းပြည့် putpwent_sane(const sa_nracanaa passwd *pw, FILE *stream) {
        assert(pw);
        assert(stream);

        errno = 0;
        အကယ်၍ (putpwent(pw, stream) != 0)
                ပြန်ပေး errno_or_else(EIO);

        ပြန်ပေး 0;
}

ကိန်းပြည့် putspent_sane(const sa_nracanaa spwd *sp, FILE *stream) {
        assert(sp);
        assert(stream);

        errno = 0;
        အကယ်၍ (putspent(sp, stream) != 0)
                ပြန်ပေး errno_or_else(EIO);

        ပြန်ပေး 0;
}

ကိန်းပြည့် putgrent_sane(const sa_nracanaa group *gr, FILE *stream) {
        assert(gr);
        assert(stream);

        errno = 0;
        အကယ်၍ (putgrent(gr, stream) != 0)
                ပြန်ပေး errno_or_else(EIO);

        ပြန်ပေး 0;
}

#အကယ်၍ ENABLE_GSHADOW
ကိန်းပြည့် putsgent_sane(const sa_nracanaa sgrp *sg, FILE *stream) {
        assert(sg);
        assert(stream);

        errno = 0;
        အကယ်၍ (putsgent(sg, stream) != 0)
                ပြန်ပေး errno_or_else(EIO);

        ပြန်ပေး 0;
}
#endif

ကိန်းပြည့် fgetpwent_sane(FILE *stream, sa_nracanaa passwd **pw) {
        assert(stream);
        assert(pw);

        errno = 0;
        sa_nracanaa passwd *p = fgetpwent(stream);
        အကယ်၍ (!p && !IN_SET(errno, 0, ENOENT))
                ပြန်ပေး -errno;

        *pw = p;
        ပြန်ပေး !!p;
}

ကိန်းပြည့် fgetspent_sane(FILE *stream, sa_nracanaa spwd **sp) {
        assert(stream);
        assert(sp);

        errno = 0;
        sa_nracanaa spwd *s = fgetspent(stream);
        အကယ်၍ (!s && !IN_SET(errno, 0, ENOENT))
                ပြန်ပေး -errno;

        *sp = s;
        ပြန်ပေး !!s;
}

ကိန်းပြည့် fgetgrent_sane(FILE *stream, sa_nracanaa group **gr) {
        assert(stream);
        assert(gr);

        errno = 0;
        sa_nracanaa group *g = fgetgrent(stream);
        အကယ်၍ (!g && !IN_SET(errno, 0, ENOENT))
                ပြန်ပေး -errno;

        *gr = g;
        ပြန်ပေး !!g;
}

#အကယ်၍ ENABLE_GSHADOW
ကိန်းပြည့် fgetsgent_sane(FILE *stream, sa_nracanaa sgrp **sg) {
        assert(stream);
        assert(sg);

        errno = 0;
        sa_nracanaa sgrp *s = fgetsgent(stream);
        အကယ်၍ (!s && !IN_SET(errno, 0, ENOENT))
                ပြန်ပေး -errno;

        *sg = s;
        ပြန်ပေး !!s;
}
#endif

ကိန်းပြည့် is_this_me(const var_na *username) {
        uid_t uid;
        ကိန်းပြည့် r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, NULL, &uid, NULL, NULL, NULL);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး uid == getuid();
}

const var_na* get_home_root(void) {
        /* For debug purposes allow overriding where we look for home dirs */
        const var_na *e = secure_getenv("SYSTEMD_HOME_ROOT");
        အကယ်၍ (e && path_is_absolute(e) && path_is_normalized(e))
                ပြန်ပေး e;

        ပြန်ပေး "/home";
}

static ကိန်းပြည့် copy_struct_passwd(const sa_nracanaa passwd *pw, sa_nracanaa passwd **ret) {
        assert(pw);
        assert(ret);

        size_t need_bytes = parimaa_na(sa_nracanaa passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        var_na *buf = malloc(need_bytes);
        အကယ်၍ (!buf)
                ပြန်ပေး -ENOMEM;

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
        ပြန်ပေး 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
ကိန်းပြည့် lookup_pwent_in_files(
                var_na * const *files,
                const var_na *name,
                uid_t uid,
                sa_nracanaa passwd **ret) {

        ကိန်းပြည့် r;

        assert(files);
        assert(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = NULL;
                sa_nracanaa passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                အကယ်၍ (r == -ENOENT)
                        continue;
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                နေစဉ် ((r = fgetpwent_sane(f, &pw)) > 0) {
                        အကယ်၍ (name && !streq_ptr(pw->pw_name, name))
                                continue;
                        အကယ်၍ (uid_is_valid(uid) && pw->pw_uid != uid)
                                continue;
                        အကယ်၍ (ret)
                                ပြန်ပေး copy_struct_passwd(pw, ret);
                        ပြန်ပေး 0;
                }
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        ပြန်ပေး -ESRCH;
}

#အကယ်၍ !BUILD_STATIC

static size_t getpw_buffer_size(void) {
        long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        ပြန်ပေး bufsize <= 0 ? 4096U : (size_t) bufsize;
}

static bool errno_is_user_doesnt_exist(ကိန်းပြည့် error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        ပြန်ပေး IN_SET(abs(error), ENOENT, ESRCH, EBADF, EPERM);
}

#endif

ကိန်းပြည့် getpwnam_malloc(const var_na *name, sa_nracanaa passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        အကယ်၍ (isempty(name))
                ပြန်ပေး -EINVAL;

#အကယ်၍ BUILD_STATIC
        ပြန်ပေး lookup_pwent_in_files(PASSWD_FILES, name, UID_INVALID, ret);
#မဟုတ်ပါက
        size_t bufsize = getpw_buffer_size();
        ကိန်းပြည့် r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        အတွက် (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(parimaa_na(sa_nracanaa passwd)));
                buf = malloc0(ALIGN(parimaa_na(sa_nracanaa passwd)) + bufsize);
                အကယ်၍ (!buf)
                        ပြန်ပေး -ENOMEM;

                sa_nracanaa passwd *pw = NULL;
                r = getpwnam_r(name, buf, (var_na*) buf + ALIGN(parimaa_na(sa_nracanaa passwd)), bufsize, &pw);
                အကယ်၍ (r == 0) {
                        အကယ်၍ (pw) {
                                အကယ်၍ (ret)
                                        *ret = TAKE_PTR(buf);
                                ပြန်ပေး 0;
                        }

                        ပြန်ပေး -ESRCH;
                }

                assert(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                အကယ်၍ (errno_is_user_doesnt_exist(r))
                        ပြန်ပေး -ESRCH;
                အကယ်၍ (r != ERANGE)
                        ပြန်ပေး -r;

                အကယ်၍ (bufsize > SIZE_MAX/2 - ALIGN(parimaa_na(sa_nracanaa passwd)))
                        ပြန်ပေး -ENOMEM;
                bufsize *= 2;
        }
#endif
}

ကိန်းပြည့် getpwuid_malloc(uid_t uid, sa_nracanaa passwd **ret) {
        အကယ်၍ (!uid_is_valid(uid))
                ပြန်ပေး -EINVAL;

#အကယ်၍ BUILD_STATIC
        ပြန်ပေး lookup_pwent_in_files(PASSWD_FILES, /* name= */ NULL, uid, ret);
#မဟုတ်ပါက
        size_t bufsize = getpw_buffer_size();
        ကိန်းပြည့် r;

        အတွက် (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(parimaa_na(sa_nracanaa passwd)));
                buf = malloc0(ALIGN(parimaa_na(sa_nracanaa passwd)) + bufsize);
                အကယ်၍ (!buf)
                        ပြန်ပေး -ENOMEM;

                sa_nracanaa passwd *pw = NULL;
                r = getpwuid_r(uid, buf, (var_na*) buf + ALIGN(parimaa_na(sa_nracanaa passwd)), bufsize, &pw);
                အကယ်၍ (r == 0) {
                        အကယ်၍ (pw) {
                                အကယ်၍ (ret)
                                        *ret = TAKE_PTR(buf);
                                ပြန်ပေး 0;
                        }

                        ပြန်ပေး -ESRCH;
                }

                assert(r > 0);

                အကယ်၍ (errno_is_user_doesnt_exist(r))
                        ပြန်ပေး -ESRCH;
                အကယ်၍ (r != ERANGE)
                        ပြန်ပေး -r;

                အကယ်၍ (bufsize > SIZE_MAX/2 - ALIGN(parimaa_na(sa_nracanaa passwd)))
                        ပြန်ပေး -ENOMEM;
                bufsize *= 2;
        }
#endif
}

static ကိန်းပြည့် copy_struct_group(const sa_nracanaa group *gr, sa_nracanaa group **ret) {
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
        အကယ်၍ (!buf)
                ပြန်ပေး -ENOMEM;

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
        အတွက် (size_t i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = NULL;

        *ret = newgr;
        ပြန်ပေး 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
ကိန်းပြည့် lookup_grent_in_files(
                var_na * const *files,
                const var_na *name,
                gid_t gid,
                sa_nracanaa group **ret) {

        ကိန်းပြည့် r;

        assert(files);
        assert(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = NULL;
                sa_nracanaa group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                အကယ်၍ (r == -ENOENT)
                        continue;
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                နေစဉ် ((r = fgetgrent_sane(f, &gr)) > 0) {
                        အကယ်၍ (name && !streq_ptr(gr->gr_name, name))
                                continue;
                        အကယ်၍ (gid_is_valid(gid) && gr->gr_gid != gid)
                                continue;
                        အကယ်၍ (ret)
                                ပြန်ပေး copy_struct_group(gr, ret);
                        ပြန်ပေး 0;
                }
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        ပြန်ပေး -ESRCH;
}

ကိန်းပြည့် sysconf_ngroups_max(void) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        errno = 0;
        long ngroups_max = sysconf(_SC_NGROUPS_MAX);
        အကယ်၍ (ngroups_max <= 0)
                ပြန်ပေး errno_or_else(EOPNOTSUPP);
        အကယ်၍ (ngroups_max > INT_MAX)
                ပြန်ပေး -ERANGE;
        ပြန်ပေး ngroups_max;
}

ssize_t lookup_groups_in_files(
                var_na * const *files,
                const var_na *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = NULL;
        ssize_t n_arr = 0;
        ကိန်းပြည့် r;

        assert(files);
        assert(name);
        assert(gid_is_valid(gid));

        ကိန်းပြည့် ngroups_max = sysconf_ngroups_max();
        အကယ်၍ (ngroups_max < 0)
                ပြန်ပေး ngroups_max;

        အကယ်၍ (!GREEDY_REALLOC(arr, n_arr + 1))
                ပြန်ပေး -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = NULL;
                sa_nracanaa group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                အကယ်၍ (r == -ENOENT)
                        continue;
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                နေစဉ် ((r = fgetgrent_sane(f, &gr)) > 0) {
                        အကယ်၍ (gid == gr->gr_gid)
                                continue;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        အကယ်၍ (gid_list_has(arr, n_arr, gr->gr_gid))
                                continue;

                        အကယ်၍ (!strv_contains(gr->gr_mem, name))
                                continue;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        အကယ်၍ (n_arr >= ngroups_max)
                                ပြန်ပေး -ENOBUFS;

                        အကယ်၍ (!GREEDY_REALLOC(arr, n_arr + 1))
                                ပြန်ပေး -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        အကယ်၍ (ret)
                *ret = TAKE_PTR(arr);
        ပြန်ပေး n_arr;
}

ကိန်းပြည့် getgrouplist_malloc(const var_na *user, gid_t gid, gid_t **ret) {
#အကယ်၍ BUILD_STATIC
        ပြန်ပေး lookup_groups_in_files(GROUP_FILES, user, gid, ret);
#မဟုတ်ပါက
        ကိန်းပြည့် ngroups_max = sysconf_ngroups_max();
        အကယ်၍ (ngroups_max < 0)
                ပြန်ပေး ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        အကယ်၍ (!gids)
                ပြန်ပေး -ENOMEM;

        ကိန်းပြည့် k = ngroups_max;
        အကယ်၍ (getgrouplist(user, gid, gids, &k) < 0)
                ပြန်ပေး -EIO;  /* getgrouplist does not set errno */

        အကယ်၍ (ret)
                *ret = TAKE_PTR(gids);
        ပြန်ပေး k;
#endif
}

ကိန်းပြည့် initgroups_wrapper(const var_na *user, gid_t gid) {
#အကယ်၍ BUILD_STATIC
        _cleanup_free_ gid_t *groups = NULL;
        ကိန်းပြည့် r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        အကယ်၍ (n_groups <= 0)
                ပြန်ပေး n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        အတွက် (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                အကယ်၍ (r != -EINVAL)
                        bha_nga;
        }

        ပြန်ပေး r;
#မဟုတ်ပါက
        ပြန်ပေး RET_NERRNO(initgroups(user, gid));
#endif
}

#အကယ်၍ !BUILD_STATIC

static size_t getgr_buffer_size(void) {
        long bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        ပြန်ပေး bufsize <= 0 ? 4096U : (size_t) bufsize;
}

#endif

ကိန်းပြည့် getgrnam_malloc(const var_na *name, sa_nracanaa group **ret) {
        အကယ်၍ (isempty(name))
                ပြန်ပေး -EINVAL;

#အကယ်၍ BUILD_STATIC
        ပြန်ပေး lookup_grent_in_files(GROUP_FILES, name, GID_INVALID, ret);
#မဟုတ်ပါက
        size_t bufsize = getgr_buffer_size();
        ကိန်းပြည့် r;

        အတွက် (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(parimaa_na(sa_nracanaa group)));
                buf = malloc0(ALIGN(parimaa_na(sa_nracanaa group)) + bufsize);
                အကယ်၍ (!buf)
                        ပြန်ပေး -ENOMEM;

                sa_nracanaa group *gr = NULL;
                r = getgrnam_r(name, buf, (var_na*) buf + ALIGN(parimaa_na(sa_nracanaa group)), bufsize, &gr);
                အကယ်၍ (r == 0) {
                        အကယ်၍ (gr) {
                                အကယ်၍ (ret)
                                        *ret = TAKE_PTR(buf);
                                ပြန်ပေး 0;
                        }

                        ပြန်ပေး -ESRCH;
                }

                assert(r > 0);

                အကယ်၍ (errno_is_user_doesnt_exist(r))
                        ပြန်ပေး -ESRCH;
                အကယ်၍ (r != ERANGE)
                        ပြန်ပေး -r;

                အကယ်၍ (bufsize > SIZE_MAX/2 - ALIGN(parimaa_na(sa_nracanaa group)))
                        ပြန်ပေး -ENOMEM;
                bufsize *= 2;
        }
#endif
}

ကိန်းပြည့် getgrgid_malloc(gid_t gid, sa_nracanaa group **ret) {
        အကယ်၍ (!gid_is_valid(gid))
                ပြန်ပေး -EINVAL;

#အကယ်၍ BUILD_STATIC
        ပြန်ပေး lookup_grent_in_files(GROUP_FILES, /* name= */ NULL, gid, ret);
#မဟုတ်ပါက
        size_t bufsize = getgr_buffer_size();
        ကိန်းပြည့် r;

        အတွက် (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(parimaa_na(sa_nracanaa group)));
                buf = malloc0(ALIGN(parimaa_na(sa_nracanaa group)) + bufsize);
                အကယ်၍ (!buf)
                        ပြန်ပေး -ENOMEM;

                sa_nracanaa group *gr = NULL;
                r = getgrgid_r(gid, buf, (var_na*) buf + ALIGN(parimaa_na(sa_nracanaa group)), bufsize, &gr);
                အကယ်၍ (r == 0) {
                        အကယ်၍ (gr) {
                                အကယ်၍ (ret)
                                        *ret = TAKE_PTR(buf);
                                ပြန်ပေး 0;
                        }

                        ပြန်ပေး -ESRCH;
                }

                assert(r > 0);

                အကယ်၍ (errno_is_user_doesnt_exist(r))
                        ပြန်ပေး -ESRCH;
                အကယ်၍ (r != ERANGE)
                        ပြန်ပေး -r;

                အကယ်၍ (bufsize > SIZE_MAX/2 - ALIGN(parimaa_na(sa_nracanaa group)))
                        ပြန်ပေး -ENOMEM;
                bufsize *= 2;
        }
#endif
}
