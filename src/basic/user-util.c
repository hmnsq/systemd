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
        const char* strerror_##type(                                    \
                        số_nguyên errnum,                                     \
                        char *buf,                                      \
                        size_t buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                switch (errnum) {                                       \
                case ESRCH:                                             \
                        trả_về "Unknown " STRINGIFY(type);              \
                case ENOEXEC:                                           \
                        trả_về "Not a system " STRINGIFY(type);         \
                default:                                                \
                        trả_về strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        nếu (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                trả_về false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        nếu (uid == (uid_t) UINT32_C(0xFFFF))
                trả_về false;

        trả_về true;
}

số_nguyên parse_uid(const char *s, uid_t *ret) {
        uint32_t uid = 0;
        số_nguyên r;

        assert(s);

        assert_cc(sizeof(uid_t) == sizeof(uint32_t));

        /* We are very strict when parsing UIDs, and prohibit +/- as prefix, leading zero as prefix, and
         * whitespace. We do this, since this call is often used in a context where we parse things as UID
         * first, and if that doesn't work we fall back to NSS. Thus we really want to make sure that UIDs
         * are parsed as UIDs only if they really really look like UIDs. */
        r = safe_atou32_full(s, 10
                             | SAFE_ATO_REFUSE_PLUS_MINUS
                             | SAFE_ATO_REFUSE_LEADING_ZERO
                             | SAFE_ATO_REFUSE_LEADING_WHITESPACE, &uid);
        nếu (r < 0)
                trả_về r;

        nếu (!uid_is_valid(uid))
                trả_về -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        nếu (ret)
                *ret = uid;

        trả_về 0;
}

số_nguyên parse_uid_range(const char *s, uid_t *ret_lower, uid_t *ret_upper) {
        _cleanup_free_ char *word = NULL;
        uid_t l, u;
        số_nguyên r;

        assert(s);
        assert(ret_lower);
        assert(ret_upper);

        r = extract_first_word(&s, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        nếu (r < 0)
                trả_về r;
        nếu (r == 0)
                trả_về -EINVAL;

        r = parse_uid(word, &l);
        nếu (r < 0)
                trả_về r;

        /* Check for the upper bound and extract it if needed */
        nếu (!s)
                /* Single number with no dash. */
                u = l;
        không_thì nếu (!*s)
                /* Trailing dash is an error. */
                trả_về -EINVAL;
        không_thì {
                r = parse_uid(s, &u);
                nếu (r < 0)
                        trả_về r;

                nếu (l > u)
                        trả_về -EINVAL;
        }

        *ret_lower = l;
        *ret_upper = u;
        trả_về 0;
}

char* getlogname_malloc(void) {
        uid_t uid;
        struct stat st;

        nếu (isatty_safe(STDIN_FILENO) && fstat(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        không_thì
                uid = getuid();

        trả_về uid_to_name(uid);
}

char* getusername_malloc(void) {
        const char *e;

        e = secure_getenv("USER");
        nếu (e)
                trả_về strdup(e);

        trả_về uid_to_name(getuid());
}

bool is_nologin_shell(const char *shell) {
        trả_về PATH_IN_SET(shell,
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

bool shell_is_placeholder(const char *shell) {
        trả_về isempty(shell) || is_nologin_shell(shell);
}

const char* default_root_shell_at(số_nguyên rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        assert(rfd >= 0 || rfd == AT_FDCWD);

        số_nguyên r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, NULL, NULL);
        nếu (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        nếu (r > 0)
                trả_về DEFAULT_USER_SHELL;

        trả_về "/bin/sh";
}

const char* default_root_shell(const char *root) {
        _cleanup_close_ số_nguyên rfd = -EBADF;

        rfd = open(empty_to_root(root), O_CLOEXEC | O_DIRECTORY | O_PATH);
        nếu (rfd < 0)
                trả_về "/bin/sh";

        trả_về default_root_shell_at(rfd);
}

static số_nguyên return_user_creds(
                const char *username,
                uid_t uid, gid_t gid,
                const char *home,
                const char *shell,
                char **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                char **ret_home,
                char **ret_shell) {
        /* Helper function to help with the strdups and atomic setting of return params. */

        _cleanup_free_ char *s1 = NULL, *s2 = NULL, *s3 = NULL;
        số_nguyên r;

        nếu (ret_username) {
                r = strdup_to(&s1, username);
                nếu (r < 0)
                        trả_về r;
        }

        nếu (ret_home) {
                r = strdup_to(&s2, home);
                nếu (r < 0)
                        trả_về r;
        }

        nếu (ret_shell) {
                r = strdup_to(&s3, shell);
                nếu (r < 0)
                        trả_về r;
        }

        nếu (ret_username)
                *ret_username = TAKE_PTR(s1);
        nếu (ret_uid)
                *ret_uid = uid;
        nếu (ret_gid)
                *ret_gid = gid;
        nếu (ret_home)
                *ret_home = TAKE_PTR(s2);
        nếu (ret_shell)
                *ret_shell = TAKE_PTR(s3);
        trả_về 0;
}

static số_nguyên synthesize_user_creds(
                const char *username,
                UserCredsFlags flags,
                char **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                char **ret_home,
                char **ret_shell) {
        assert(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        nếu (STR_IN_SET(username, "root", "0"))
                trả_về return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_root_shell(NULL) : NULL,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        nếu (STR_IN_SET(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                trả_về return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? NULL : "/",
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? NULL : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        trả_về -ENOMEDIUM;
}

số_nguyên get_user_creds(
                const char *username,
                UserCredsFlags flags,
                char **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                char **ret_home,
                char **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ struct passwd *pw = NULL;
        số_nguyên r;

        assert(username);
        assert((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        nếu (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS) ||
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
                nếu (r >= 0)
                        trả_về 0;
                nếu (r != -ENOMEDIUM) /* not a username we can synthesize */
                        trả_về r;
        }

        nếu (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                nếu (r >= 0)
                        username = pw->pw_name;

                không_thì nếu (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        nếu (ret_username)
                                *ret_username = NULL;
                        nếu (ret_uid)
                                *ret_uid = u;
                        trả_về 0;
                }
        } không_thì
                r = getpwnam_malloc(username, &pw);

        nếu (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                nếu (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        trả_về 0;

                trả_về r;
        }

        nếu (ret_uid && !uid_is_valid(pw->pw_uid))
                trả_về -EBADMSG;

        nếu (ret_gid && !gid_is_valid(pw->pw_gid))
                trả_về -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        const char *h =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && empty_or_root(pw->pw_dir)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_dir) || !path_is_absolute(pw->pw_dir)))
                ? NULL : pw->pw_dir;

        const char *s =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_shell) || !path_is_absolute(pw->pw_shell)))
                ? NULL : pw->pw_shell;

        trả_về return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

static số_nguyên synthesize_group_creds(const char *groupname, char **ret_name, gid_t *ret_gid) {
        assert(groupname);

        gid_t id;
        const char *n;
        số_nguyên r;

        nếu (STR_IN_SET(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } không_thì nếu (STR_IN_SET(groupname, NOBODY_GROUP_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_GROUP_NAME;
        } không_thì
                trả_về -ENOMEDIUM;

        r = strdup_to_full(ret_name, n);
        nếu (r < 0)
                trả_về r;
        nếu (ret_gid)
                *ret_gid = id;
        trả_về 0;
}

số_nguyên get_group_creds(const char *groupname, UserCredsFlags flags, char **ret_name, gid_t *ret_gid) {
        _cleanup_free_ struct group *gr = NULL;
        gid_t id;
        số_nguyên r;

        assert(groupname);

        nếu (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_group_creds(groupname, ret_name, ret_gid);
                nếu (r >= 0)
                        trả_về 0;
                nếu (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        trả_về r;
        }

        nếu (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                nếu (r >= 0)
                        groupname = gr->gr_name;
                không_thì nếu (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING)) {
                        nếu (ret_gid)
                                *ret_gid = id;
                        nếu (ret_name)
                                *ret_name = NULL;
                        trả_về 0;
                }
        } không_thì
                r = getgrnam_malloc(groupname, &gr);

        nếu (r < 0) {
                nếu (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_group_creds(groupname, ret_name, ret_gid) >= 0)
                        trả_về 0;
                trả_về r;
        }

        nếu (ret_gid && !gid_is_valid(gr->gr_gid))
                trả_về -EBADMSG;

        r = strdup_to_full(ret_name, groupname);
        nếu (r < 0)
                trả_về r;
        nếu (ret_gid)
                *ret_gid = gr->gr_gid;
        trả_về 0;
}

char* uid_to_name(uid_t uid) {
        char *ret;
        số_nguyên r;

        /* Shortcut things to avoid NSS lookups */
        nếu (uid == 0)
                trả_về strdup("root");
        nếu (uid == UID_NOBODY && synthesize_nobody())
                trả_về strdup(NOBODY_USER_NAME);

        nếu (uid_is_valid(uid)) {
                _cleanup_free_ struct passwd *pw = NULL;

                r = getpwuid_malloc(uid, &pw);
                nếu (r >= 0)
                        trả_về strdup(pw->pw_name);
        }

        nếu (asprintf(&ret, UID_FMT, uid) < 0)
                trả_về NULL;

        trả_về ret;
}

char* gid_to_name(gid_t gid) {
        char *ret;
        số_nguyên r;

        nếu (gid == 0)
                trả_về strdup("root");
        nếu (gid == GID_NOBODY && synthesize_nobody())
                trả_về strdup(NOBODY_GROUP_NAME);

        nếu (gid_is_valid(gid)) {
                _cleanup_free_ struct group *gr = NULL;

                r = getgrgid_malloc(gid, &gr);
                nếu (r >= 0)
                        trả_về strdup(gr->gr_name);
        }

        nếu (asprintf(&ret, GID_FMT, gid) < 0)
                trả_về NULL;

        trả_về ret;
}

static bool gid_list_has(const gid_t *list, size_t size, gid_t val) {
        assert(list || size == 0);

        FOREACH_ARRAY(i, list, size)
                nếu (*i == val)
                        trả_về true;

        trả_về false;
}

số_nguyên in_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = NULL;
        số_nguyên ngroups;

        nếu (getgid() == gid)
                trả_về 1;

        nếu (getegid() == gid)
                trả_về 1;

        nếu (!gid_is_valid(gid))
                trả_về -EINVAL;

        ngroups = getgroups_alloc(&gids);
        nếu (ngroups < 0)
                trả_về ngroups;

        trả_về gid_list_has(gids, ngroups, gid);
}

số_nguyên in_group(const char *name) {
        số_nguyên r;
        gid_t gid;

        r = get_group_creds(name, /* flags= */ 0, /* ret_name= */ NULL, &gid);
        nếu (r < 0)
                trả_về r;

        trả_về in_gid(gid);
}

số_nguyên merge_gid_lists(const gid_t *list1, size_t size1, const gid_t *list2, size_t size2, gid_t **ret) {
        size_t nresult = 0;

        assert(size1 == 0 || list1);
        assert(size2 == 0 || list2);
        assert(ret);

        nếu (size2 > INT_MAX - size1)
                trả_về -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        nếu (!buf)
                trả_về -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        cho (size_t i = 0; i < size1; i++)
                nếu (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        cho (size_t i = 0; i < size2; i++)
                nếu (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        trả_về (số_nguyên)nresult;
}

số_nguyên getgroups_alloc(gid_t **ret) {
        số_nguyên ngroups = 8;

        assert(ret);

        cho (unsigned attempt = 0;;) {
                _cleanup_free_ gid_t *p = NULL;

                p = new(gid_t, ngroups);
                nếu (!p)
                        trả_về -ENOMEM;

                ngroups = getgroups(ngroups, p);
                nếu (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        trả_về ngroups;
                }
                nếu (ngroups == 0)
                        break;
                nếu (errno != EINVAL)
                        trả_về -errno;

                /* Give up eventually */
                nếu (attempt++ > 10)
                        trả_về -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, NULL);
                nếu (ngroups < 0)
                        trả_về -errno;
                nếu (ngroups == 0)
                        break;
        }

        *ret = NULL;
        trả_về 0;
}

số_nguyên get_home_dir(char **ret) {
        _cleanup_free_ struct passwd *p = NULL;
        const char *e;
        uid_t u;
        số_nguyên r;

        assert(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        nếu (e && path_is_valid(e) && path_is_absolute(e))
                goto found;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        nếu (u == 0) {
                e = "/root";
                goto found;
        }
        nếu (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                goto found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        nếu (r < 0)
                trả_về r;

        e = p->pw_dir;
        nếu (!path_is_valid(e) || !path_is_absolute(e))
                trả_về -EINVAL;

 found:
        trả_về path_simplify_alloc(e, ret);
}

số_nguyên get_shell(char **ret) {
        _cleanup_free_ struct passwd *p = NULL;
        const char *e;
        uid_t u;
        số_nguyên r;

        assert(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        nếu (e && path_is_valid(e) && path_is_absolute(e))
                goto found;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        nếu (u == 0) {
                e = default_root_shell(NULL);
                goto found;
        }
        nếu (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                goto found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        nếu (r < 0)
                trả_về r;

        e = p->pw_shell;
        nếu (!path_is_valid(e) || !path_is_absolute(e))
                trả_về -EINVAL;

 found:
        trả_về path_simplify_alloc(e, ret);
}

số_nguyên fully_set_uid_gid(uid_t uid, gid_t gid, const gid_t supplementary_gids[], size_t n_supplementary_gids) {
        số_nguyên r;

        assert(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        nếu (r < 0)
                trả_về r;

        nếu (gid_is_valid(gid))
                nếu (setresgid(gid, gid, gid) < 0)
                        trả_về -errno;

        nếu (uid_is_valid(uid))
                nếu (setresuid(uid, uid, uid) < 0)
                        trả_về -errno;

        trả_về 0;
}

số_nguyên take_etc_passwd_lock(const char *root) {
        số_nguyên r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ char *path = path_join(root, ETC_PASSWD_LOCK_PATH);
        nếu (!path)
                trả_về log_oom_debug();

        (void) mkdir_parents(path, 0755);

        _cleanup_close_ số_nguyên fd = open(path, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        nếu (fd < 0)
                trả_về log_debug_errno(errno, "Cannot open %s: %m", path);

        r = unposix_lock(fd, LOCK_EX);
        nếu (r < 0)
                trả_về log_debug_errno(r, "Locking %s failed: %m", path);

        trả_về TAKE_FD(fd);
}

bool valid_user_group_name(const char *u, ValidUserFlags flags) {
        const char *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        nếu (isempty(u)) /* An empty user name is never valid */
                trả_về false;

        nếu (parse_uid(u, NULL) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                trả_về FLAGS_SET(flags, VALID_USER_ALLOW_NUMERIC);

        nếu (FLAGS_SET(flags, VALID_USER_RELAX)) {

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

                nếu (startswith(u, " ") || endswith(u, " ")) /* At least expect whitespace padding is removed
                                                             * at front and back (accept in the middle, since
                                                             * that's apparently a thing on Windows). Note
                                                             * that this also blocks usernames consisting of
                                                             * whitespace only. */
                        trả_về false;

                nếu (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        trả_về false;

                nếu (string_has_cc(u, NULL)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        trả_về false;

                nếu (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        trả_về false;

                nếu (in_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        trả_về false;

                nếu (u[0] == '-' && in_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        trả_về false;

                nếu (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        trả_về false;

                /* Compare with strict result and warn if result doesn't match */
                nếu (FLAGS_SET(flags, VALID_USER_WARN) && !valid_user_group_name(u, 0))
                        log_struct(LOG_NOTICE,
                                   LOG_MESSAGE("Accepting user/group name '%s', which does not match strict user/group name rules.", u),
                                   LOG_ITEM("USER_GROUP_NAME=%s", u),
                                   LOG_MESSAGE_ID(SD_MESSAGE_UNSAFE_USER_NAME_STR));

                /* Note that we make no restrictions on the length in relaxed mode! */
        } không_thì {
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

                nếu (!ascii_isalpha(u[0]) &&
                    u[0] != '_')
                        trả_về false;

                cho (i = u+1; *i; i++)
                        nếu (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !IN_SET(*i, '_', '-'))
                                trả_về false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                nếu (l > (size_t) sz) /* glibc: 256 */
                        trả_về false;
                nếu (l > NAME_MAX) /* must fit in a filename: 255 */
                        trả_về false;
                nếu (l > sizeof_field(struct utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        trả_về false;
        }

        trả_về true;
}

bool valid_gecos(const char *d) {

        nếu (!d)
                trả_về false;

        nếu (!utf8_is_valid(d))
                trả_về false;

        nếu (string_has_cc(d, NULL))
                trả_về false;

        /* Colons are used as field separators, and hence not OK */
        nếu (strchr(d, ':'))
                trả_về false;

        trả_về true;
}

char* mangle_gecos(const char *d) {
        char *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        nếu (!mangled)
                trả_về NULL;

        cho (char *i = mangled; *i; i++) {
                số_nguyên len;

                nếu ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        continue;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                nếu (len < 0) {
                        *i = ' ';
                        continue;
                }

                i += len - 1;
        }

        trả_về mangled;
}

bool valid_home(const char *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        nếu (isempty(p))
                trả_về false;

        nếu (!utf8_is_valid(p))
                trả_về false;

        nếu (string_has_cc(p, NULL))
                trả_về false;

        nếu (!path_is_absolute(p))
                trả_về false;

        nếu (!path_is_normalized(p))
                trả_về false;

        /* Colons are used as field separators, and hence not OK */
        nếu (strchr(p, ':'))
                trả_về false;

        trả_về true;
}

bool valid_shell(const char *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        nếu (!valid_home(p))
                trả_về false;

        trả_về !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

số_nguyên maybe_setgroups(size_t size, const gid_t *list) {
        số_nguyên r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        nếu (size == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ FILE *f = fopen("/proc/self/gid_map", "re");
                nếu (!f && errno != ENOENT)
                        trả_về -errno;
                nếu (f) {
                        r = safe_fgetc(f, /* ret= */ NULL);
                        nếu (r < 0)
                                trả_về r;
                        nếu (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                trả_về 0;
                        }
                }

                _cleanup_free_ char *setgroups_content = NULL;
                r = read_one_line_file("/proc/self/setgroups", &setgroups_content);
                nếu (r < 0 && r != -ENOENT)
                        trả_về r;
                nếu (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        trả_về 0;
                }
        }

        trả_về RET_NERRNO(setgroups(size, list));
}

bool synthesize_nobody(void) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        static số_nguyên cache = -1;

        nếu (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        trả_về cache;
}

số_nguyên putpwent_sane(const struct passwd *pw, FILE *stream) {
        assert(pw);
        assert(stream);

        errno = 0;
        nếu (putpwent(pw, stream) != 0)
                trả_về errno_or_else(EIO);

        trả_về 0;
}

số_nguyên putspent_sane(const struct spwd *sp, FILE *stream) {
        assert(sp);
        assert(stream);

        errno = 0;
        nếu (putspent(sp, stream) != 0)
                trả_về errno_or_else(EIO);

        trả_về 0;
}

số_nguyên putgrent_sane(const struct group *gr, FILE *stream) {
        assert(gr);
        assert(stream);

        errno = 0;
        nếu (putgrent(gr, stream) != 0)
                trả_về errno_or_else(EIO);

        trả_về 0;
}

#nếu ENABLE_GSHADOW
số_nguyên putsgent_sane(const struct sgrp *sg, FILE *stream) {
        assert(sg);
        assert(stream);

        errno = 0;
        nếu (putsgent(sg, stream) != 0)
                trả_về errno_or_else(EIO);

        trả_về 0;
}
#endif

số_nguyên fgetpwent_sane(FILE *stream, struct passwd **pw) {
        assert(stream);
        assert(pw);

        errno = 0;
        struct passwd *p = fgetpwent(stream);
        nếu (!p && !IN_SET(errno, 0, ENOENT))
                trả_về -errno;

        *pw = p;
        trả_về !!p;
}

số_nguyên fgetspent_sane(FILE *stream, struct spwd **sp) {
        assert(stream);
        assert(sp);

        errno = 0;
        struct spwd *s = fgetspent(stream);
        nếu (!s && !IN_SET(errno, 0, ENOENT))
                trả_về -errno;

        *sp = s;
        trả_về !!s;
}

số_nguyên fgetgrent_sane(FILE *stream, struct group **gr) {
        assert(stream);
        assert(gr);

        errno = 0;
        struct group *g = fgetgrent(stream);
        nếu (!g && !IN_SET(errno, 0, ENOENT))
                trả_về -errno;

        *gr = g;
        trả_về !!g;
}

#nếu ENABLE_GSHADOW
số_nguyên fgetsgent_sane(FILE *stream, struct sgrp **sg) {
        assert(stream);
        assert(sg);

        errno = 0;
        struct sgrp *s = fgetsgent(stream);
        nếu (!s && !IN_SET(errno, 0, ENOENT))
                trả_về -errno;

        *sg = s;
        trả_về !!s;
}
#endif

số_nguyên is_this_me(const char *username) {
        uid_t uid;
        số_nguyên r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, NULL, &uid, NULL, NULL, NULL);
        nếu (r < 0)
                trả_về r;

        trả_về uid == getuid();
}

const char* get_home_root(void) {
        /* For debug purposes allow overriding where we look for home dirs */
        const char *e = secure_getenv("SYSTEMD_HOME_ROOT");
        nếu (e && path_is_absolute(e) && path_is_normalized(e))
                trả_về e;

        trả_về "/home";
}

static số_nguyên copy_struct_passwd(const struct passwd *pw, struct passwd **ret) {
        assert(pw);
        assert(ret);

        size_t need_bytes = sizeof(struct passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        char *buf = malloc(need_bytes);
        nếu (!buf)
                trả_về -ENOMEM;

        struct passwd *newpw = (struct passwd *) buf;

        /* The layout in our buffer:
         * struct passwd, and then individual strings. */
        char *p = buf + sizeof(struct passwd);

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
        trả_về 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
số_nguyên lookup_pwent_in_files(
                char * const *files,
                const char *name,
                uid_t uid,
                struct passwd **ret) {

        số_nguyên r;

        assert(files);
        assert(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = NULL;
                struct passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                nếu (r == -ENOENT)
                        continue;
                nếu (r < 0)
                        trả_về r;

                trong_khi ((r = fgetpwent_sane(f, &pw)) > 0) {
                        nếu (name && !streq_ptr(pw->pw_name, name))
                                continue;
                        nếu (uid_is_valid(uid) && pw->pw_uid != uid)
                                continue;
                        nếu (ret)
                                trả_về copy_struct_passwd(pw, ret);
                        trả_về 0;
                }
                nếu (r < 0)
                        trả_về r;
        }

        trả_về -ESRCH;
}

#nếu !BUILD_STATIC

static size_t getpw_buffer_size(void) {
        long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        trả_về bufsize <= 0 ? 4096U : (size_t) bufsize;
}

static bool errno_is_user_doesnt_exist(số_nguyên error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        trả_về IN_SET(abs(error), ENOENT, ESRCH, EBADF, EPERM);
}

#endif

số_nguyên getpwnam_malloc(const char *name, struct passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        nếu (isempty(name))
                trả_về -EINVAL;

#nếu BUILD_STATIC
        trả_về lookup_pwent_in_files(PASSWD_FILES, name, UID_INVALID, ret);
#không_thì
        size_t bufsize = getpw_buffer_size();
        số_nguyên r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        cho (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(sizeof(struct passwd)));
                buf = malloc0(ALIGN(sizeof(struct passwd)) + bufsize);
                nếu (!buf)
                        trả_về -ENOMEM;

                struct passwd *pw = NULL;
                r = getpwnam_r(name, buf, (char*) buf + ALIGN(sizeof(struct passwd)), bufsize, &pw);
                nếu (r == 0) {
                        nếu (pw) {
                                nếu (ret)
                                        *ret = TAKE_PTR(buf);
                                trả_về 0;
                        }

                        trả_về -ESRCH;
                }

                assert(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                nếu (errno_is_user_doesnt_exist(r))
                        trả_về -ESRCH;
                nếu (r != ERANGE)
                        trả_về -r;

                nếu (bufsize > SIZE_MAX/2 - ALIGN(sizeof(struct passwd)))
                        trả_về -ENOMEM;
                bufsize *= 2;
        }
#endif
}

số_nguyên getpwuid_malloc(uid_t uid, struct passwd **ret) {
        nếu (!uid_is_valid(uid))
                trả_về -EINVAL;

#nếu BUILD_STATIC
        trả_về lookup_pwent_in_files(PASSWD_FILES, /* name= */ NULL, uid, ret);
#không_thì
        size_t bufsize = getpw_buffer_size();
        số_nguyên r;

        cho (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(sizeof(struct passwd)));
                buf = malloc0(ALIGN(sizeof(struct passwd)) + bufsize);
                nếu (!buf)
                        trả_về -ENOMEM;

                struct passwd *pw = NULL;
                r = getpwuid_r(uid, buf, (char*) buf + ALIGN(sizeof(struct passwd)), bufsize, &pw);
                nếu (r == 0) {
                        nếu (pw) {
                                nếu (ret)
                                        *ret = TAKE_PTR(buf);
                                trả_về 0;
                        }

                        trả_về -ESRCH;
                }

                assert(r > 0);

                nếu (errno_is_user_doesnt_exist(r))
                        trả_về -ESRCH;
                nếu (r != ERANGE)
                        trả_về -r;

                nếu (bufsize > SIZE_MAX/2 - ALIGN(sizeof(struct passwd)))
                        trả_về -ENOMEM;
                bufsize *= 2;
        }
#endif
}

static số_nguyên copy_struct_group(const struct group *gr, struct group **ret) {
        assert(gr);
        assert(ret);

        size_t need_bytes = sizeof(struct group)
                + strlen_ptr(gr->gr_name) + 1
                + strlen_ptr(gr->gr_passwd) + 1,
                n_mem = 0;
        STRV_FOREACH(s, gr->gr_mem) {
                need_bytes += sizeof(char*) + strlen(*s) + 1;
                n_mem++;
        }
        need_bytes += sizeof(char*);  /* NULL terminator for gr_mem */

        char *buf = malloc(need_bytes);
        nếu (!buf)
                trả_về -ENOMEM;

        struct group *newgr = (struct group *) buf;

        /* The layout in our buffer:
         * struct group, ->gr_mem pointers terminated by NULL, ->gr_name, ->gr_passwd, ->gr_mem items */
        /* The ->gr_mem array is first, because it needs alignment. */
        assert_cc(sizeof(struct group) % alignof(char*) == 0);

        char *p = buf + sizeof(struct group) + (n_mem + 1) * sizeof(char*);

        newgr->gr_name = p;
        p = stpcpy(p, strempty(gr->gr_name)) + 1;

        newgr->gr_passwd = p;
        p = stpcpy(p, strempty(gr->gr_passwd)) + 1;

        newgr->gr_gid = gr->gr_gid;

        newgr->gr_mem = (char**) (buf + sizeof(struct group));
        cho (size_t i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = NULL;

        *ret = newgr;
        trả_về 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
số_nguyên lookup_grent_in_files(
                char * const *files,
                const char *name,
                gid_t gid,
                struct group **ret) {

        số_nguyên r;

        assert(files);
        assert(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = NULL;
                struct group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                nếu (r == -ENOENT)
                        continue;
                nếu (r < 0)
                        trả_về r;

                trong_khi ((r = fgetgrent_sane(f, &gr)) > 0) {
                        nếu (name && !streq_ptr(gr->gr_name, name))
                                continue;
                        nếu (gid_is_valid(gid) && gr->gr_gid != gid)
                                continue;
                        nếu (ret)
                                trả_về copy_struct_group(gr, ret);
                        trả_về 0;
                }
                nếu (r < 0)
                        trả_về r;
        }

        trả_về -ESRCH;
}

số_nguyên sysconf_ngroups_max(void) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        errno = 0;
        long ngroups_max = sysconf(_SC_NGROUPS_MAX);
        nếu (ngroups_max <= 0)
                trả_về errno_or_else(EOPNOTSUPP);
        nếu (ngroups_max > INT_MAX)
                trả_về -ERANGE;
        trả_về ngroups_max;
}

ssize_t lookup_groups_in_files(
                char * const *files,
                const char *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = NULL;
        ssize_t n_arr = 0;
        số_nguyên r;

        assert(files);
        assert(name);
        assert(gid_is_valid(gid));

        số_nguyên ngroups_max = sysconf_ngroups_max();
        nếu (ngroups_max < 0)
                trả_về ngroups_max;

        nếu (!GREEDY_REALLOC(arr, n_arr + 1))
                trả_về -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = NULL;
                struct group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                nếu (r == -ENOENT)
                        continue;
                nếu (r < 0)
                        trả_về r;

                trong_khi ((r = fgetgrent_sane(f, &gr)) > 0) {
                        nếu (gid == gr->gr_gid)
                                continue;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        nếu (gid_list_has(arr, n_arr, gr->gr_gid))
                                continue;

                        nếu (!strv_contains(gr->gr_mem, name))
                                continue;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        nếu (n_arr >= ngroups_max)
                                trả_về -ENOBUFS;

                        nếu (!GREEDY_REALLOC(arr, n_arr + 1))
                                trả_về -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                nếu (r < 0)
                        trả_về r;
        }

        nếu (ret)
                *ret = TAKE_PTR(arr);
        trả_về n_arr;
}

số_nguyên getgrouplist_malloc(const char *user, gid_t gid, gid_t **ret) {
#nếu BUILD_STATIC
        trả_về lookup_groups_in_files(GROUP_FILES, user, gid, ret);
#không_thì
        số_nguyên ngroups_max = sysconf_ngroups_max();
        nếu (ngroups_max < 0)
                trả_về ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        nếu (!gids)
                trả_về -ENOMEM;

        số_nguyên k = ngroups_max;
        nếu (getgrouplist(user, gid, gids, &k) < 0)
                trả_về -EIO;  /* getgrouplist does not set errno */

        nếu (ret)
                *ret = TAKE_PTR(gids);
        trả_về k;
#endif
}

số_nguyên initgroups_wrapper(const char *user, gid_t gid) {
#nếu BUILD_STATIC
        _cleanup_free_ gid_t *groups = NULL;
        số_nguyên r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        nếu (n_groups <= 0)
                trả_về n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        cho (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                nếu (r != -EINVAL)
                        break;
        }

        trả_về r;
#không_thì
        trả_về RET_NERRNO(initgroups(user, gid));
#endif
}

#nếu !BUILD_STATIC

static size_t getgr_buffer_size(void) {
        long bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        trả_về bufsize <= 0 ? 4096U : (size_t) bufsize;
}

#endif

số_nguyên getgrnam_malloc(const char *name, struct group **ret) {
        nếu (isempty(name))
                trả_về -EINVAL;

#nếu BUILD_STATIC
        trả_về lookup_grent_in_files(GROUP_FILES, name, GID_INVALID, ret);
#không_thì
        size_t bufsize = getgr_buffer_size();
        số_nguyên r;

        cho (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(sizeof(struct group)));
                buf = malloc0(ALIGN(sizeof(struct group)) + bufsize);
                nếu (!buf)
                        trả_về -ENOMEM;

                struct group *gr = NULL;
                r = getgrnam_r(name, buf, (char*) buf + ALIGN(sizeof(struct group)), bufsize, &gr);
                nếu (r == 0) {
                        nếu (gr) {
                                nếu (ret)
                                        *ret = TAKE_PTR(buf);
                                trả_về 0;
                        }

                        trả_về -ESRCH;
                }

                assert(r > 0);

                nếu (errno_is_user_doesnt_exist(r))
                        trả_về -ESRCH;
                nếu (r != ERANGE)
                        trả_về -r;

                nếu (bufsize > SIZE_MAX/2 - ALIGN(sizeof(struct group)))
                        trả_về -ENOMEM;
                bufsize *= 2;
        }
#endif
}

số_nguyên getgrgid_malloc(gid_t gid, struct group **ret) {
        nếu (!gid_is_valid(gid))
                trả_về -EINVAL;

#nếu BUILD_STATIC
        trả_về lookup_grent_in_files(GROUP_FILES, /* name= */ NULL, gid, ret);
#không_thì
        size_t bufsize = getgr_buffer_size();
        số_nguyên r;

        cho (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(sizeof(struct group)));
                buf = malloc0(ALIGN(sizeof(struct group)) + bufsize);
                nếu (!buf)
                        trả_về -ENOMEM;

                struct group *gr = NULL;
                r = getgrgid_r(gid, buf, (char*) buf + ALIGN(sizeof(struct group)), bufsize, &gr);
                nếu (r == 0) {
                        nếu (gr) {
                                nếu (ret)
                                        *ret = TAKE_PTR(buf);
                                trả_về 0;
                        }

                        trả_về -ESRCH;
                }

                assert(r > 0);

                nếu (errno_is_user_doesnt_exist(r))
                        trả_về -ESRCH;
                nếu (r != ERANGE)
                        trả_về -r;

                nếu (bufsize > SIZE_MAX/2 - ALIGN(sizeof(struct group)))
                        trả_về -ENOMEM;
                bufsize *= 2;
        }
#endif
}
