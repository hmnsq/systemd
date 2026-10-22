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
                        පූර්ණ_සංඛ්‍යා errnum,                                     \
                        var_na *buf,                                      \
                        size_t buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                switch (errnum) {                                       \
                pak_sa ESRCH:                                             \
                        ආපසු "Unknown " STRINGIFY(type);              \
                pak_sa ENOEXEC:                                           \
                        ආපසු "Not a system " STRINGIFY(type);         \
                default:                                                \
                        ආපසු strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        නම් (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                ආපසු false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        නම් (uid == (uid_t) UINT32_C(0xFFFF))
                ආපසු false;

        ආපසු true;
}

පූර්ණ_සංඛ්‍යා parse_uid(const var_na *s, uid_t *ret) {
        uint32_t uid = 0;
        පූර්ණ_සංඛ්‍යා r;

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
        නම් (r < 0)
                ආපසු r;

        නම් (!uid_is_valid(uid))
                ආපසු -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        නම් (ret)
                *ret = uid;

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා parse_uid_range(const var_na *s, uid_t *ret_lower, uid_t *ret_upper) {
        _cleanup_free_ var_na *word = NULL;
        uid_t l, u;
        පූර්ණ_සංඛ්‍යා r;

        assert(s);
        assert(ret_lower);
        assert(ret_upper);

        r = extract_first_word(&s, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        නම් (r < 0)
                ආපසු r;
        නම් (r == 0)
                ආපසු -EINVAL;

        r = parse_uid(word, &l);
        නම් (r < 0)
                ආපසු r;

        /* Check for the upper bound and extract it if needed */
        නම් (!s)
                /* Single number with no dash. */
                u = l;
        නැතහොත් නම් (!*s)
                /* Trailing dash is an error. */
                ආපසු -EINVAL;
        නැතහොත් {
                r = parse_uid(s, &u);
                නම් (r < 0)
                        ආපසු r;

                නම් (l > u)
                        ආපසු -EINVAL;
        }

        *ret_lower = l;
        *ret_upper = u;
        ආපසු 0;
}

var_na* getlogname_malloc(void) {
        uid_t uid;
        sa_nracanaa stat st;

        නම් (isatty_safe(STDIN_FILENO) && fstat(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        නැතහොත්
                uid = getuid();

        ආපසු uid_to_name(uid);
}

var_na* getusername_malloc(void) {
        const var_na *e;

        e = secure_getenv("USER");
        නම් (e)
                ආපසු strdup(e);

        ආපසු uid_to_name(getuid());
}

bool is_nologin_shell(const var_na *shell) {
        ආපසු PATH_IN_SET(shell,
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
        ආපසු isempty(shell) || is_nologin_shell(shell);
}

const var_na* default_root_shell_at(පූර්ණ_සංඛ්‍යා rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        assert(rfd >= 0 || rfd == AT_FDCWD);

        පූර්ණ_සංඛ්‍යා r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, NULL, NULL);
        නම් (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        නම් (r > 0)
                ආපසු DEFAULT_USER_SHELL;

        ආපසු "/bin/sh";
}

const var_na* default_root_shell(const var_na *root) {
        _cleanup_close_ පූර්ණ_සංඛ්‍යා rfd = -EBADF;

        rfd = open(empty_to_root(root), O_CLOEXEC | O_DIRECTORY | O_PATH);
        නම් (rfd < 0)
                ආපසු "/bin/sh";

        ආපසු default_root_shell_at(rfd);
}

static පූර්ණ_සංඛ්‍යා return_user_creds(
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
        පූර්ණ_සංඛ්‍යා r;

        නම් (ret_username) {
                r = strdup_to(&s1, username);
                නම් (r < 0)
                        ආපසු r;
        }

        නම් (ret_home) {
                r = strdup_to(&s2, home);
                නම් (r < 0)
                        ආපසු r;
        }

        නම් (ret_shell) {
                r = strdup_to(&s3, shell);
                නම් (r < 0)
                        ආපසු r;
        }

        නම් (ret_username)
                *ret_username = TAKE_PTR(s1);
        නම් (ret_uid)
                *ret_uid = uid;
        නම් (ret_gid)
                *ret_gid = gid;
        නම් (ret_home)
                *ret_home = TAKE_PTR(s2);
        නම් (ret_shell)
                *ret_shell = TAKE_PTR(s3);
        ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා synthesize_user_creds(
                const var_na *username,
                UserCredsFlags flags,
                var_na **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                var_na **ret_home,
                var_na **ret_shell) {
        assert(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        නම් (STR_IN_SET(username, "root", "0"))
                ආපසු return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_root_shell(NULL) : NULL,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        නම් (STR_IN_SET(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                ආපසු return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? NULL : "/",
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? NULL : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        ආපසු -ENOMEDIUM;
}

පූර්ණ_සංඛ්‍යා get_user_creds(
                const var_na *username,
                UserCredsFlags flags,
                var_na **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                var_na **ret_home,
                var_na **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ sa_nracanaa passwd *pw = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(username);
        assert((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        නම් (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS) ||
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
                නම් (r >= 0)
                        ආපසු 0;
                නම් (r != -ENOMEDIUM) /* not a username we can synthesize */
                        ආපසු r;
        }

        නම් (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                නම් (r >= 0)
                        username = pw->pw_name;

                නැතහොත් නම් (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        නම් (ret_username)
                                *ret_username = NULL;
                        නම් (ret_uid)
                                *ret_uid = u;
                        ආපසු 0;
                }
        } නැතහොත්
                r = getpwnam_malloc(username, &pw);

        නම් (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                නම් (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        ආපසු 0;

                ආපසු r;
        }

        නම් (ret_uid && !uid_is_valid(pw->pw_uid))
                ආපසු -EBADMSG;

        නම් (ret_gid && !gid_is_valid(pw->pw_gid))
                ආපසු -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        const var_na *h =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && empty_or_root(pw->pw_dir)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_dir) || !path_is_absolute(pw->pw_dir)))
                ? NULL : pw->pw_dir;

        const var_na *s =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_shell) || !path_is_absolute(pw->pw_shell)))
                ? NULL : pw->pw_shell;

        ආපසු return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

static පූර්ණ_සංඛ්‍යා synthesize_group_creds(const var_na *groupname, var_na **ret_name, gid_t *ret_gid) {
        assert(groupname);

        gid_t id;
        const var_na *n;
        පූර්ණ_සංඛ්‍යා r;

        නම් (STR_IN_SET(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } නැතහොත් නම් (STR_IN_SET(groupname, NOBODY_GROUP_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_GROUP_NAME;
        } නැතහොත්
                ආපසු -ENOMEDIUM;

        r = strdup_to_full(ret_name, n);
        නම් (r < 0)
                ආපසු r;
        නම් (ret_gid)
                *ret_gid = id;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා get_group_creds(const var_na *groupname, UserCredsFlags flags, var_na **ret_name, gid_t *ret_gid) {
        _cleanup_free_ sa_nracanaa group *gr = NULL;
        gid_t id;
        පූර්ණ_සංඛ්‍යා r;

        assert(groupname);

        නම් (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_group_creds(groupname, ret_name, ret_gid);
                නම් (r >= 0)
                        ආපසු 0;
                නම් (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        ආපසු r;
        }

        නම් (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                නම් (r >= 0)
                        groupname = gr->gr_name;
                නැතහොත් නම් (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING)) {
                        නම් (ret_gid)
                                *ret_gid = id;
                        නම් (ret_name)
                                *ret_name = NULL;
                        ආපසු 0;
                }
        } නැතහොත්
                r = getgrnam_malloc(groupname, &gr);

        නම් (r < 0) {
                නම් (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_group_creds(groupname, ret_name, ret_gid) >= 0)
                        ආපසු 0;
                ආපසු r;
        }

        නම් (ret_gid && !gid_is_valid(gr->gr_gid))
                ආපසු -EBADMSG;

        r = strdup_to_full(ret_name, groupname);
        නම් (r < 0)
                ආපසු r;
        නම් (ret_gid)
                *ret_gid = gr->gr_gid;
        ආපසු 0;
}

var_na* uid_to_name(uid_t uid) {
        var_na *ret;
        පූර්ණ_සංඛ්‍යා r;

        /* Shortcut things to avoid NSS lookups */
        නම් (uid == 0)
                ආපසු strdup("root");
        නම් (uid == UID_NOBODY && synthesize_nobody())
                ආපසු strdup(NOBODY_USER_NAME);

        නම් (uid_is_valid(uid)) {
                _cleanup_free_ sa_nracanaa passwd *pw = NULL;

                r = getpwuid_malloc(uid, &pw);
                නම් (r >= 0)
                        ආපසු strdup(pw->pw_name);
        }

        නම් (asprintf(&ret, UID_FMT, uid) < 0)
                ආපසු NULL;

        ආපසු ret;
}

var_na* gid_to_name(gid_t gid) {
        var_na *ret;
        පූර්ණ_සංඛ්‍යා r;

        නම් (gid == 0)
                ආපසු strdup("root");
        නම් (gid == GID_NOBODY && synthesize_nobody())
                ආපසු strdup(NOBODY_GROUP_NAME);

        නම් (gid_is_valid(gid)) {
                _cleanup_free_ sa_nracanaa group *gr = NULL;

                r = getgrgid_malloc(gid, &gr);
                නම් (r >= 0)
                        ආපසු strdup(gr->gr_name);
        }

        නම් (asprintf(&ret, GID_FMT, gid) < 0)
                ආපසු NULL;

        ආපසු ret;
}

static bool gid_list_has(const gid_t *list, size_t size, gid_t val) {
        assert(list || size == 0);

        FOREACH_ARRAY(i, list, size)
                නම් (*i == val)
                        ආපසු true;

        ආපසු false;
}

පූර්ණ_සංඛ්‍යා in_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = NULL;
        පූර්ණ_සංඛ්‍යා ngroups;

        නම් (getgid() == gid)
                ආපසු 1;

        නම් (getegid() == gid)
                ආපසු 1;

        නම් (!gid_is_valid(gid))
                ආපසු -EINVAL;

        ngroups = getgroups_alloc(&gids);
        නම් (ngroups < 0)
                ආපසු ngroups;

        ආපසු gid_list_has(gids, ngroups, gid);
}

පූර්ණ_සංඛ්‍යා in_group(const var_na *name) {
        පූර්ණ_සංඛ්‍යා r;
        gid_t gid;

        r = get_group_creds(name, /* flags= */ 0, /* ret_name= */ NULL, &gid);
        නම් (r < 0)
                ආපසු r;

        ආපසු in_gid(gid);
}

පූර්ණ_සංඛ්‍යා merge_gid_lists(const gid_t *list1, size_t size1, const gid_t *list2, size_t size2, gid_t **ret) {
        size_t nresult = 0;

        assert(size1 == 0 || list1);
        assert(size2 == 0 || list2);
        assert(ret);

        නම් (size2 > INT_MAX - size1)
                ආපසු -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        නම් (!buf)
                ආපසු -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        සඳහා (size_t i = 0; i < size1; i++)
                නම් (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        සඳහා (size_t i = 0; i < size2; i++)
                නම් (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        ආපසු (පූර්ණ_සංඛ්‍යා)nresult;
}

පූර්ණ_සංඛ්‍යා getgroups_alloc(gid_t **ret) {
        පූර්ණ_සංඛ්‍යා ngroups = 8;

        assert(ret);

        සඳහා (unsigned attempt = 0;;) {
                _cleanup_free_ gid_t *p = NULL;

                p = new(gid_t, ngroups);
                නම් (!p)
                        ආපසු -ENOMEM;

                ngroups = getgroups(ngroups, p);
                නම් (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        ආපසු ngroups;
                }
                නම් (ngroups == 0)
                        bha_nga;
                නම් (errno != EINVAL)
                        ආපසු -errno;

                /* Give up eventually */
                නම් (attempt++ > 10)
                        ආපසු -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, NULL);
                නම් (ngroups < 0)
                        ආපසු -errno;
                නම් (ngroups == 0)
                        bha_nga;
        }

        *ret = NULL;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා get_home_dir(var_na **ret) {
        _cleanup_free_ sa_nracanaa passwd *p = NULL;
        const var_na *e;
        uid_t u;
        පූර්ණ_සංඛ්‍යා r;

        assert(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        නම් (e && path_is_valid(e) && path_is_absolute(e))
                goto found;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        නම් (u == 0) {
                e = "/root";
                goto found;
        }
        නම් (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                goto found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        නම් (r < 0)
                ආපසු r;

        e = p->pw_dir;
        නම් (!path_is_valid(e) || !path_is_absolute(e))
                ආපසු -EINVAL;

 found:
        ආපසු path_simplify_alloc(e, ret);
}

පූර්ණ_සංඛ්‍යා get_shell(var_na **ret) {
        _cleanup_free_ sa_nracanaa passwd *p = NULL;
        const var_na *e;
        uid_t u;
        පූර්ණ_සංඛ්‍යා r;

        assert(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        නම් (e && path_is_valid(e) && path_is_absolute(e))
                goto found;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        නම් (u == 0) {
                e = default_root_shell(NULL);
                goto found;
        }
        නම් (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                goto found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        නම් (r < 0)
                ආපසු r;

        e = p->pw_shell;
        නම් (!path_is_valid(e) || !path_is_absolute(e))
                ආපසු -EINVAL;

 found:
        ආපසු path_simplify_alloc(e, ret);
}

පූර්ණ_සංඛ්‍යා fully_set_uid_gid(uid_t uid, gid_t gid, const gid_t supplementary_gids[], size_t n_supplementary_gids) {
        පූර්ණ_සංඛ්‍යා r;

        assert(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        නම් (r < 0)
                ආපසු r;

        නම් (gid_is_valid(gid))
                නම් (setresgid(gid, gid, gid) < 0)
                        ආපසු -errno;

        නම් (uid_is_valid(uid))
                නම් (setresuid(uid, uid, uid) < 0)
                        ආපසු -errno;

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා take_etc_passwd_lock(const var_na *root) {
        පූර්ණ_සංඛ්‍යා r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ var_na *path = path_join(root, ETC_PASSWD_LOCK_PATH);
        නම් (!path)
                ආපසු log_oom_debug();

        (void) mkdir_parents(path, 0755);

        _cleanup_close_ පූර්ණ_සංඛ්‍යා fd = open(path, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        නම් (fd < 0)
                ආපසු log_debug_errno(errno, "Cannot open %s: %m", path);

        r = unposix_lock(fd, LOCK_EX);
        නම් (r < 0)
                ආපසු log_debug_errno(r, "Locking %s failed: %m", path);

        ආපසු TAKE_FD(fd);
}

bool valid_user_group_name(const var_na *u, ValidUserFlags flags) {
        const var_na *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        නම් (isempty(u)) /* An empty user name is never valid */
                ආපසු false;

        නම් (parse_uid(u, NULL) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                ආපසු FLAGS_SET(flags, VALID_USER_ALLOW_NUMERIC);

        නම් (FLAGS_SET(flags, VALID_USER_RELAX)) {

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

                නම් (startswith(u, " ") || endswith(u, " ")) /* At least expect whitespace padding is removed
                                                             * at front and back (accept in the middle, since
                                                             * that's apparently a thing on Windows). Note
                                                             * that this also blocks usernames consisting of
                                                             * whitespace only. */
                        ආපසු false;

                නම් (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        ආපසු false;

                නම් (string_has_cc(u, NULL)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        ආපසු false;

                නම් (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        ආපසු false;

                නම් (in_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        ආපසු false;

                නම් (u[0] == '-' && in_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        ආපසු false;

                නම් (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        ආපසු false;

                /* Compare with strict result and warn if result doesn't match */
                නම් (FLAGS_SET(flags, VALID_USER_WARN) && !valid_user_group_name(u, 0))
                        log_struct(LOG_NOTICE,
                                   LOG_MESSAGE("Accepting user/group name '%s', which does not match strict user/group name rules.", u),
                                   LOG_ITEM("USER_GROUP_NAME=%s", u),
                                   LOG_MESSAGE_ID(SD_MESSAGE_UNSAFE_USER_NAME_STR));

                /* Note that we make no restrictions on the length in relaxed mode! */
        } නැතහොත් {
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

                නම් (!ascii_isalpha(u[0]) &&
                    u[0] != '_')
                        ආපසු false;

                සඳහා (i = u+1; *i; i++)
                        නම් (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !IN_SET(*i, '_', '-'))
                                ආපසු false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                නම් (l > (size_t) sz) /* glibc: 256 */
                        ආපසු false;
                නම් (l > NAME_MAX) /* must fit in a filename: 255 */
                        ආපසු false;
                නම් (l > sizeof_field(sa_nracanaa utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        ආපසු false;
        }

        ආපසු true;
}

bool valid_gecos(const var_na *d) {

        නම් (!d)
                ආපසු false;

        නම් (!utf8_is_valid(d))
                ආපසු false;

        නම් (string_has_cc(d, NULL))
                ආපසු false;

        /* Colons are used as field separators, and hence not OK */
        නම් (strchr(d, ':'))
                ආපසු false;

        ආපසු true;
}

var_na* mangle_gecos(const var_na *d) {
        var_na *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        නම් (!mangled)
                ආපසු NULL;

        සඳහා (var_na *i = mangled; *i; i++) {
                පූර්ණ_සංඛ්‍යා len;

                නම් ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        continue;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                නම් (len < 0) {
                        *i = ' ';
                        continue;
                }

                i += len - 1;
        }

        ආපසු mangled;
}

bool valid_home(const var_na *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        නම් (isempty(p))
                ආපසු false;

        නම් (!utf8_is_valid(p))
                ආපසු false;

        නම් (string_has_cc(p, NULL))
                ආපසු false;

        නම් (!path_is_absolute(p))
                ආපසු false;

        නම් (!path_is_normalized(p))
                ආපසු false;

        /* Colons are used as field separators, and hence not OK */
        නම් (strchr(p, ':'))
                ආපසු false;

        ආපසු true;
}

bool valid_shell(const var_na *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        නම් (!valid_home(p))
                ආපසු false;

        ආපසු !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

පූර්ණ_සංඛ්‍යා maybe_setgroups(size_t size, const gid_t *list) {
        පූර්ණ_සංඛ්‍යා r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        නම් (size == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ FILE *f = fopen("/proc/self/gid_map", "re");
                නම් (!f && errno != ENOENT)
                        ආපසු -errno;
                නම් (f) {
                        r = safe_fgetc(f, /* ret= */ NULL);
                        නම් (r < 0)
                                ආපසු r;
                        නම් (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                ආපසු 0;
                        }
                }

                _cleanup_free_ var_na *setgroups_content = NULL;
                r = read_one_line_file("/proc/self/setgroups", &setgroups_content);
                නම් (r < 0 && r != -ENOENT)
                        ආපසු r;
                නම් (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        ආපසු 0;
                }
        }

        ආපසු RET_NERRNO(setgroups(size, list));
}

bool synthesize_nobody(void) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        static පූර්ණ_සංඛ්‍යා cache = -1;

        නම් (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        ආපසු cache;
}

පූර්ණ_සංඛ්‍යා putpwent_sane(const sa_nracanaa passwd *pw, FILE *stream) {
        assert(pw);
        assert(stream);

        errno = 0;
        නම් (putpwent(pw, stream) != 0)
                ආපසු errno_or_else(EIO);

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා putspent_sane(const sa_nracanaa spwd *sp, FILE *stream) {
        assert(sp);
        assert(stream);

        errno = 0;
        නම් (putspent(sp, stream) != 0)
                ආපසු errno_or_else(EIO);

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා putgrent_sane(const sa_nracanaa group *gr, FILE *stream) {
        assert(gr);
        assert(stream);

        errno = 0;
        නම් (putgrent(gr, stream) != 0)
                ආපසු errno_or_else(EIO);

        ආපසු 0;
}

#නම් ENABLE_GSHADOW
පූර්ණ_සංඛ්‍යා putsgent_sane(const sa_nracanaa sgrp *sg, FILE *stream) {
        assert(sg);
        assert(stream);

        errno = 0;
        නම් (putsgent(sg, stream) != 0)
                ආපසු errno_or_else(EIO);

        ආපසු 0;
}
#endif

පූර්ණ_සංඛ්‍යා fgetpwent_sane(FILE *stream, sa_nracanaa passwd **pw) {
        assert(stream);
        assert(pw);

        errno = 0;
        sa_nracanaa passwd *p = fgetpwent(stream);
        නම් (!p && !IN_SET(errno, 0, ENOENT))
                ආපසු -errno;

        *pw = p;
        ආපසු !!p;
}

පූර්ණ_සංඛ්‍යා fgetspent_sane(FILE *stream, sa_nracanaa spwd **sp) {
        assert(stream);
        assert(sp);

        errno = 0;
        sa_nracanaa spwd *s = fgetspent(stream);
        නම් (!s && !IN_SET(errno, 0, ENOENT))
                ආපසු -errno;

        *sp = s;
        ආපසු !!s;
}

පූර්ණ_සංඛ්‍යා fgetgrent_sane(FILE *stream, sa_nracanaa group **gr) {
        assert(stream);
        assert(gr);

        errno = 0;
        sa_nracanaa group *g = fgetgrent(stream);
        නම් (!g && !IN_SET(errno, 0, ENOENT))
                ආපසු -errno;

        *gr = g;
        ආපසු !!g;
}

#නම් ENABLE_GSHADOW
පූර්ණ_සංඛ්‍යා fgetsgent_sane(FILE *stream, sa_nracanaa sgrp **sg) {
        assert(stream);
        assert(sg);

        errno = 0;
        sa_nracanaa sgrp *s = fgetsgent(stream);
        නම් (!s && !IN_SET(errno, 0, ENOENT))
                ආපසු -errno;

        *sg = s;
        ආපසු !!s;
}
#endif

පූර්ණ_සංඛ්‍යා is_this_me(const var_na *username) {
        uid_t uid;
        පූර්ණ_සංඛ්‍යා r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, NULL, &uid, NULL, NULL, NULL);
        නම් (r < 0)
                ආපසු r;

        ආපසු uid == getuid();
}

const var_na* get_home_root(void) {
        /* For debug purposes allow overriding where we look for home dirs */
        const var_na *e = secure_getenv("SYSTEMD_HOME_ROOT");
        නම් (e && path_is_absolute(e) && path_is_normalized(e))
                ආපසු e;

        ආපසු "/home";
}

static පූර්ණ_සංඛ්‍යා copy_struct_passwd(const sa_nracanaa passwd *pw, sa_nracanaa passwd **ret) {
        assert(pw);
        assert(ret);

        size_t need_bytes = parimaa_na(sa_nracanaa passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        var_na *buf = malloc(need_bytes);
        නම් (!buf)
                ආපසු -ENOMEM;

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
        ආපසු 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
පූර්ණ_සංඛ්‍යා lookup_pwent_in_files(
                var_na * const *files,
                const var_na *name,
                uid_t uid,
                sa_nracanaa passwd **ret) {

        පූර්ණ_සංඛ්‍යා r;

        assert(files);
        assert(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = NULL;
                sa_nracanaa passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                නම් (r == -ENOENT)
                        continue;
                නම් (r < 0)
                        ආපසු r;

                අතරතුර ((r = fgetpwent_sane(f, &pw)) > 0) {
                        නම් (name && !streq_ptr(pw->pw_name, name))
                                continue;
                        නම් (uid_is_valid(uid) && pw->pw_uid != uid)
                                continue;
                        නම් (ret)
                                ආපසු copy_struct_passwd(pw, ret);
                        ආපසු 0;
                }
                නම් (r < 0)
                        ආපසු r;
        }

        ආපසු -ESRCH;
}

#නම් !BUILD_STATIC

static size_t getpw_buffer_size(void) {
        long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        ආපසු bufsize <= 0 ? 4096U : (size_t) bufsize;
}

static bool errno_is_user_doesnt_exist(පූර්ණ_සංඛ්‍යා error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        ආපසු IN_SET(abs(error), ENOENT, ESRCH, EBADF, EPERM);
}

#endif

පූර්ණ_සංඛ්‍යා getpwnam_malloc(const var_na *name, sa_nracanaa passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        නම් (isempty(name))
                ආපසු -EINVAL;

#නම් BUILD_STATIC
        ආපසු lookup_pwent_in_files(PASSWD_FILES, name, UID_INVALID, ret);
#නැතහොත්
        size_t bufsize = getpw_buffer_size();
        පූර්ණ_සංඛ්‍යා r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        සඳහා (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(parimaa_na(sa_nracanaa passwd)));
                buf = malloc0(ALIGN(parimaa_na(sa_nracanaa passwd)) + bufsize);
                නම් (!buf)
                        ආපසු -ENOMEM;

                sa_nracanaa passwd *pw = NULL;
                r = getpwnam_r(name, buf, (var_na*) buf + ALIGN(parimaa_na(sa_nracanaa passwd)), bufsize, &pw);
                නම් (r == 0) {
                        නම් (pw) {
                                නම් (ret)
                                        *ret = TAKE_PTR(buf);
                                ආපසු 0;
                        }

                        ආපසු -ESRCH;
                }

                assert(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                නම් (errno_is_user_doesnt_exist(r))
                        ආපසු -ESRCH;
                නම් (r != ERANGE)
                        ආපසු -r;

                නම් (bufsize > SIZE_MAX/2 - ALIGN(parimaa_na(sa_nracanaa passwd)))
                        ආපසු -ENOMEM;
                bufsize *= 2;
        }
#endif
}

පූර්ණ_සංඛ්‍යා getpwuid_malloc(uid_t uid, sa_nracanaa passwd **ret) {
        නම් (!uid_is_valid(uid))
                ආපසු -EINVAL;

#නම් BUILD_STATIC
        ආපසු lookup_pwent_in_files(PASSWD_FILES, /* name= */ NULL, uid, ret);
#නැතහොත්
        size_t bufsize = getpw_buffer_size();
        පූර්ණ_සංඛ්‍යා r;

        සඳහා (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(parimaa_na(sa_nracanaa passwd)));
                buf = malloc0(ALIGN(parimaa_na(sa_nracanaa passwd)) + bufsize);
                නම් (!buf)
                        ආපසු -ENOMEM;

                sa_nracanaa passwd *pw = NULL;
                r = getpwuid_r(uid, buf, (var_na*) buf + ALIGN(parimaa_na(sa_nracanaa passwd)), bufsize, &pw);
                නම් (r == 0) {
                        නම් (pw) {
                                නම් (ret)
                                        *ret = TAKE_PTR(buf);
                                ආපසු 0;
                        }

                        ආපසු -ESRCH;
                }

                assert(r > 0);

                නම් (errno_is_user_doesnt_exist(r))
                        ආපසු -ESRCH;
                නම් (r != ERANGE)
                        ආපසු -r;

                නම් (bufsize > SIZE_MAX/2 - ALIGN(parimaa_na(sa_nracanaa passwd)))
                        ආපසු -ENOMEM;
                bufsize *= 2;
        }
#endif
}

static පූර්ණ_සංඛ්‍යා copy_struct_group(const sa_nracanaa group *gr, sa_nracanaa group **ret) {
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
        නම් (!buf)
                ආපසු -ENOMEM;

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
        සඳහා (size_t i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = NULL;

        *ret = newgr;
        ආපසු 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
පූර්ණ_සංඛ්‍යා lookup_grent_in_files(
                var_na * const *files,
                const var_na *name,
                gid_t gid,
                sa_nracanaa group **ret) {

        පූර්ණ_සංඛ්‍යා r;

        assert(files);
        assert(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = NULL;
                sa_nracanaa group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                නම් (r == -ENOENT)
                        continue;
                නම් (r < 0)
                        ආපසු r;

                අතරතුර ((r = fgetgrent_sane(f, &gr)) > 0) {
                        නම් (name && !streq_ptr(gr->gr_name, name))
                                continue;
                        නම් (gid_is_valid(gid) && gr->gr_gid != gid)
                                continue;
                        නම් (ret)
                                ආපසු copy_struct_group(gr, ret);
                        ආපසු 0;
                }
                නම් (r < 0)
                        ආපසු r;
        }

        ආපසු -ESRCH;
}

පූර්ණ_සංඛ්‍යා sysconf_ngroups_max(void) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        errno = 0;
        long ngroups_max = sysconf(_SC_NGROUPS_MAX);
        නම් (ngroups_max <= 0)
                ආපසු errno_or_else(EOPNOTSUPP);
        නම් (ngroups_max > INT_MAX)
                ආපසු -ERANGE;
        ආපසු ngroups_max;
}

ssize_t lookup_groups_in_files(
                var_na * const *files,
                const var_na *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = NULL;
        ssize_t n_arr = 0;
        පූර්ණ_සංඛ්‍යා r;

        assert(files);
        assert(name);
        assert(gid_is_valid(gid));

        පූර්ණ_සංඛ්‍යා ngroups_max = sysconf_ngroups_max();
        නම් (ngroups_max < 0)
                ආපසු ngroups_max;

        නම් (!GREEDY_REALLOC(arr, n_arr + 1))
                ආපසු -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = NULL;
                sa_nracanaa group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                නම් (r == -ENOENT)
                        continue;
                නම් (r < 0)
                        ආපසු r;

                අතරතුර ((r = fgetgrent_sane(f, &gr)) > 0) {
                        නම් (gid == gr->gr_gid)
                                continue;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        නම් (gid_list_has(arr, n_arr, gr->gr_gid))
                                continue;

                        නම් (!strv_contains(gr->gr_mem, name))
                                continue;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        නම් (n_arr >= ngroups_max)
                                ආපසු -ENOBUFS;

                        නම් (!GREEDY_REALLOC(arr, n_arr + 1))
                                ආපසු -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                නම් (r < 0)
                        ආපසු r;
        }

        නම් (ret)
                *ret = TAKE_PTR(arr);
        ආපසු n_arr;
}

පූර්ණ_සංඛ්‍යා getgrouplist_malloc(const var_na *user, gid_t gid, gid_t **ret) {
#නම් BUILD_STATIC
        ආපසු lookup_groups_in_files(GROUP_FILES, user, gid, ret);
#නැතහොත්
        පූර්ණ_සංඛ්‍යා ngroups_max = sysconf_ngroups_max();
        නම් (ngroups_max < 0)
                ආපසු ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        නම් (!gids)
                ආපසු -ENOMEM;

        පූර්ණ_සංඛ්‍යා k = ngroups_max;
        නම් (getgrouplist(user, gid, gids, &k) < 0)
                ආපසු -EIO;  /* getgrouplist does not set errno */

        නම් (ret)
                *ret = TAKE_PTR(gids);
        ආපසු k;
#endif
}

පූර්ණ_සංඛ්‍යා initgroups_wrapper(const var_na *user, gid_t gid) {
#නම් BUILD_STATIC
        _cleanup_free_ gid_t *groups = NULL;
        පූර්ණ_සංඛ්‍යා r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        නම් (n_groups <= 0)
                ආපසු n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        සඳහා (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                නම් (r != -EINVAL)
                        bha_nga;
        }

        ආපසු r;
#නැතහොත්
        ආපසු RET_NERRNO(initgroups(user, gid));
#endif
}

#නම් !BUILD_STATIC

static size_t getgr_buffer_size(void) {
        long bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        ආපසු bufsize <= 0 ? 4096U : (size_t) bufsize;
}

#endif

පූර්ණ_සංඛ්‍යා getgrnam_malloc(const var_na *name, sa_nracanaa group **ret) {
        නම් (isempty(name))
                ආපසු -EINVAL;

#නම් BUILD_STATIC
        ආපසු lookup_grent_in_files(GROUP_FILES, name, GID_INVALID, ret);
#නැතහොත්
        size_t bufsize = getgr_buffer_size();
        පූර්ණ_සංඛ්‍යා r;

        සඳහා (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(parimaa_na(sa_nracanaa group)));
                buf = malloc0(ALIGN(parimaa_na(sa_nracanaa group)) + bufsize);
                නම් (!buf)
                        ආපසු -ENOMEM;

                sa_nracanaa group *gr = NULL;
                r = getgrnam_r(name, buf, (var_na*) buf + ALIGN(parimaa_na(sa_nracanaa group)), bufsize, &gr);
                නම් (r == 0) {
                        නම් (gr) {
                                නම් (ret)
                                        *ret = TAKE_PTR(buf);
                                ආපසු 0;
                        }

                        ආපසු -ESRCH;
                }

                assert(r > 0);

                නම් (errno_is_user_doesnt_exist(r))
                        ආපසු -ESRCH;
                නම් (r != ERANGE)
                        ආපසු -r;

                නම් (bufsize > SIZE_MAX/2 - ALIGN(parimaa_na(sa_nracanaa group)))
                        ආපසු -ENOMEM;
                bufsize *= 2;
        }
#endif
}

පූර්ණ_සංඛ්‍යා getgrgid_malloc(gid_t gid, sa_nracanaa group **ret) {
        නම් (!gid_is_valid(gid))
                ආපසු -EINVAL;

#නම් BUILD_STATIC
        ආපසු lookup_grent_in_files(GROUP_FILES, /* name= */ NULL, gid, ret);
#නැතහොත්
        size_t bufsize = getgr_buffer_size();
        පූර්ණ_සංඛ්‍යා r;

        සඳහා (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(parimaa_na(sa_nracanaa group)));
                buf = malloc0(ALIGN(parimaa_na(sa_nracanaa group)) + bufsize);
                නම් (!buf)
                        ආපසු -ENOMEM;

                sa_nracanaa group *gr = NULL;
                r = getgrgid_r(gid, buf, (var_na*) buf + ALIGN(parimaa_na(sa_nracanaa group)), bufsize, &gr);
                නම් (r == 0) {
                        නම් (gr) {
                                නම් (ret)
                                        *ret = TAKE_PTR(buf);
                                ආපසු 0;
                        }

                        ආපසු -ESRCH;
                }

                assert(r > 0);

                නම් (errno_is_user_doesnt_exist(r))
                        ආපසු -ESRCH;
                නම් (r != ERANGE)
                        ආපසු -r;

                නම් (bufsize > SIZE_MAX/2 - ALIGN(parimaa_na(sa_nracanaa group)))
                        ආපසු -ENOMEM;
                bufsize *= 2;
        }
#endif
}
