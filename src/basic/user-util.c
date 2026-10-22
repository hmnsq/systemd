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
                        ціле errnum,                                     \
                        char *buf,                                      \
                        size_t buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                switch (errnum) {                                       \
                case ESRCH:                                             \
                        повернути "Unknown " STRINGIFY(type);              \
                case ENOEXEC:                                           \
                        повернути "Not a system " STRINGIFY(type);         \
                default:                                                \
                        повернути strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        якщо (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                повернути false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        якщо (uid == (uid_t) UINT32_C(0xFFFF))
                повернути false;

        повернути true;
}

ціле parse_uid(const char *s, uid_t *ret) {
        uint32_t uid = 0;
        ціле r;

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
        якщо (r < 0)
                повернути r;

        якщо (!uid_is_valid(uid))
                повернути -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        якщо (ret)
                *ret = uid;

        повернути 0;
}

ціле parse_uid_range(const char *s, uid_t *ret_lower, uid_t *ret_upper) {
        _cleanup_free_ char *word = NULL;
        uid_t l, u;
        ціле r;

        assert(s);
        assert(ret_lower);
        assert(ret_upper);

        r = extract_first_word(&s, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        якщо (r < 0)
                повернути r;
        якщо (r == 0)
                повернути -EINVAL;

        r = parse_uid(word, &l);
        якщо (r < 0)
                повернути r;

        /* Check for the upper bound and extract it if needed */
        якщо (!s)
                /* Single number with no dash. */
                u = l;
        інакше якщо (!*s)
                /* Trailing dash is an error. */
                повернути -EINVAL;
        інакше {
                r = parse_uid(s, &u);
                якщо (r < 0)
                        повернути r;

                якщо (l > u)
                        повернути -EINVAL;
        }

        *ret_lower = l;
        *ret_upper = u;
        повернути 0;
}

char* getlogname_malloc(void) {
        uid_t uid;
        struct stat st;

        якщо (isatty_safe(STDIN_FILENO) && fstat(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        інакше
                uid = getuid();

        повернути uid_to_name(uid);
}

char* getusername_malloc(void) {
        const char *e;

        e = secure_getenv("USER");
        якщо (e)
                повернути strdup(e);

        повернути uid_to_name(getuid());
}

bool is_nologin_shell(const char *shell) {
        повернути PATH_IN_SET(shell,
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
        повернути isempty(shell) || is_nologin_shell(shell);
}

const char* default_root_shell_at(ціле rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        assert(rfd >= 0 || rfd == AT_FDCWD);

        ціле r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, NULL, NULL);
        якщо (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        якщо (r > 0)
                повернути DEFAULT_USER_SHELL;

        повернути "/bin/sh";
}

const char* default_root_shell(const char *root) {
        _cleanup_close_ ціле rfd = -EBADF;

        rfd = open(empty_to_root(root), O_CLOEXEC | O_DIRECTORY | O_PATH);
        якщо (rfd < 0)
                повернути "/bin/sh";

        повернути default_root_shell_at(rfd);
}

static ціле return_user_creds(
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
        ціле r;

        якщо (ret_username) {
                r = strdup_to(&s1, username);
                якщо (r < 0)
                        повернути r;
        }

        якщо (ret_home) {
                r = strdup_to(&s2, home);
                якщо (r < 0)
                        повернути r;
        }

        якщо (ret_shell) {
                r = strdup_to(&s3, shell);
                якщо (r < 0)
                        повернути r;
        }

        якщо (ret_username)
                *ret_username = TAKE_PTR(s1);
        якщо (ret_uid)
                *ret_uid = uid;
        якщо (ret_gid)
                *ret_gid = gid;
        якщо (ret_home)
                *ret_home = TAKE_PTR(s2);
        якщо (ret_shell)
                *ret_shell = TAKE_PTR(s3);
        повернути 0;
}

static ціле synthesize_user_creds(
                const char *username,
                UserCredsFlags flags,
                char **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                char **ret_home,
                char **ret_shell) {
        assert(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        якщо (STR_IN_SET(username, "root", "0"))
                повернути return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_root_shell(NULL) : NULL,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        якщо (STR_IN_SET(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                повернути return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? NULL : "/",
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? NULL : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        повернути -ENOMEDIUM;
}

ціле get_user_creds(
                const char *username,
                UserCredsFlags flags,
                char **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                char **ret_home,
                char **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ struct passwd *pw = NULL;
        ціле r;

        assert(username);
        assert((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        якщо (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS) ||
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
                якщо (r >= 0)
                        повернути 0;
                якщо (r != -ENOMEDIUM) /* not a username we can synthesize */
                        повернути r;
        }

        якщо (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                якщо (r >= 0)
                        username = pw->pw_name;

                інакше якщо (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        якщо (ret_username)
                                *ret_username = NULL;
                        якщо (ret_uid)
                                *ret_uid = u;
                        повернути 0;
                }
        } інакше
                r = getpwnam_malloc(username, &pw);

        якщо (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                якщо (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        повернути 0;

                повернути r;
        }

        якщо (ret_uid && !uid_is_valid(pw->pw_uid))
                повернути -EBADMSG;

        якщо (ret_gid && !gid_is_valid(pw->pw_gid))
                повернути -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        const char *h =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && empty_or_root(pw->pw_dir)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_dir) || !path_is_absolute(pw->pw_dir)))
                ? NULL : pw->pw_dir;

        const char *s =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_shell) || !path_is_absolute(pw->pw_shell)))
                ? NULL : pw->pw_shell;

        повернути return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

static ціле synthesize_group_creds(const char *groupname, char **ret_name, gid_t *ret_gid) {
        assert(groupname);

        gid_t id;
        const char *n;
        ціле r;

        якщо (STR_IN_SET(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } інакше якщо (STR_IN_SET(groupname, NOBODY_GROUP_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_GROUP_NAME;
        } інакше
                повернути -ENOMEDIUM;

        r = strdup_to_full(ret_name, n);
        якщо (r < 0)
                повернути r;
        якщо (ret_gid)
                *ret_gid = id;
        повернути 0;
}

ціле get_group_creds(const char *groupname, UserCredsFlags flags, char **ret_name, gid_t *ret_gid) {
        _cleanup_free_ struct group *gr = NULL;
        gid_t id;
        ціле r;

        assert(groupname);

        якщо (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_group_creds(groupname, ret_name, ret_gid);
                якщо (r >= 0)
                        повернути 0;
                якщо (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        повернути r;
        }

        якщо (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                якщо (r >= 0)
                        groupname = gr->gr_name;
                інакше якщо (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING)) {
                        якщо (ret_gid)
                                *ret_gid = id;
                        якщо (ret_name)
                                *ret_name = NULL;
                        повернути 0;
                }
        } інакше
                r = getgrnam_malloc(groupname, &gr);

        якщо (r < 0) {
                якщо (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_group_creds(groupname, ret_name, ret_gid) >= 0)
                        повернути 0;
                повернути r;
        }

        якщо (ret_gid && !gid_is_valid(gr->gr_gid))
                повернути -EBADMSG;

        r = strdup_to_full(ret_name, groupname);
        якщо (r < 0)
                повернути r;
        якщо (ret_gid)
                *ret_gid = gr->gr_gid;
        повернути 0;
}

char* uid_to_name(uid_t uid) {
        char *ret;
        ціле r;

        /* Shortcut things to avoid NSS lookups */
        якщо (uid == 0)
                повернути strdup("root");
        якщо (uid == UID_NOBODY && synthesize_nobody())
                повернути strdup(NOBODY_USER_NAME);

        якщо (uid_is_valid(uid)) {
                _cleanup_free_ struct passwd *pw = NULL;

                r = getpwuid_malloc(uid, &pw);
                якщо (r >= 0)
                        повернути strdup(pw->pw_name);
        }

        якщо (asprintf(&ret, UID_FMT, uid) < 0)
                повернути NULL;

        повернути ret;
}

char* gid_to_name(gid_t gid) {
        char *ret;
        ціле r;

        якщо (gid == 0)
                повернути strdup("root");
        якщо (gid == GID_NOBODY && synthesize_nobody())
                повернути strdup(NOBODY_GROUP_NAME);

        якщо (gid_is_valid(gid)) {
                _cleanup_free_ struct group *gr = NULL;

                r = getgrgid_malloc(gid, &gr);
                якщо (r >= 0)
                        повернути strdup(gr->gr_name);
        }

        якщо (asprintf(&ret, GID_FMT, gid) < 0)
                повернути NULL;

        повернути ret;
}

static bool gid_list_has(const gid_t *list, size_t size, gid_t val) {
        assert(list || size == 0);

        FOREACH_ARRAY(i, list, size)
                якщо (*i == val)
                        повернути true;

        повернути false;
}

ціле in_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = NULL;
        ціле ngroups;

        якщо (getgid() == gid)
                повернути 1;

        якщо (getegid() == gid)
                повернути 1;

        якщо (!gid_is_valid(gid))
                повернути -EINVAL;

        ngroups = getgroups_alloc(&gids);
        якщо (ngroups < 0)
                повернути ngroups;

        повернути gid_list_has(gids, ngroups, gid);
}

ціле in_group(const char *name) {
        ціле r;
        gid_t gid;

        r = get_group_creds(name, /* flags= */ 0, /* ret_name= */ NULL, &gid);
        якщо (r < 0)
                повернути r;

        повернути in_gid(gid);
}

ціле merge_gid_lists(const gid_t *list1, size_t size1, const gid_t *list2, size_t size2, gid_t **ret) {
        size_t nresult = 0;

        assert(size1 == 0 || list1);
        assert(size2 == 0 || list2);
        assert(ret);

        якщо (size2 > INT_MAX - size1)
                повернути -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        якщо (!buf)
                повернути -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        для (size_t i = 0; i < size1; i++)
                якщо (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        для (size_t i = 0; i < size2; i++)
                якщо (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        повернути (ціле)nresult;
}

ціле getgroups_alloc(gid_t **ret) {
        ціле ngroups = 8;

        assert(ret);

        для (unsigned attempt = 0;;) {
                _cleanup_free_ gid_t *p = NULL;

                p = new(gid_t, ngroups);
                якщо (!p)
                        повернути -ENOMEM;

                ngroups = getgroups(ngroups, p);
                якщо (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        повернути ngroups;
                }
                якщо (ngroups == 0)
                        break;
                якщо (errno != EINVAL)
                        повернути -errno;

                /* Give up eventually */
                якщо (attempt++ > 10)
                        повернути -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, NULL);
                якщо (ngroups < 0)
                        повернути -errno;
                якщо (ngroups == 0)
                        break;
        }

        *ret = NULL;
        повернути 0;
}

ціле get_home_dir(char **ret) {
        _cleanup_free_ struct passwd *p = NULL;
        const char *e;
        uid_t u;
        ціле r;

        assert(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        якщо (e && path_is_valid(e) && path_is_absolute(e))
                goto found;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        якщо (u == 0) {
                e = "/root";
                goto found;
        }
        якщо (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                goto found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        якщо (r < 0)
                повернути r;

        e = p->pw_dir;
        якщо (!path_is_valid(e) || !path_is_absolute(e))
                повернути -EINVAL;

 found:
        повернути path_simplify_alloc(e, ret);
}

ціле get_shell(char **ret) {
        _cleanup_free_ struct passwd *p = NULL;
        const char *e;
        uid_t u;
        ціле r;

        assert(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        якщо (e && path_is_valid(e) && path_is_absolute(e))
                goto found;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        якщо (u == 0) {
                e = default_root_shell(NULL);
                goto found;
        }
        якщо (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                goto found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        якщо (r < 0)
                повернути r;

        e = p->pw_shell;
        якщо (!path_is_valid(e) || !path_is_absolute(e))
                повернути -EINVAL;

 found:
        повернути path_simplify_alloc(e, ret);
}

ціле fully_set_uid_gid(uid_t uid, gid_t gid, const gid_t supplementary_gids[], size_t n_supplementary_gids) {
        ціле r;

        assert(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        якщо (r < 0)
                повернути r;

        якщо (gid_is_valid(gid))
                якщо (setresgid(gid, gid, gid) < 0)
                        повернути -errno;

        якщо (uid_is_valid(uid))
                якщо (setresuid(uid, uid, uid) < 0)
                        повернути -errno;

        повернути 0;
}

ціле take_etc_passwd_lock(const char *root) {
        ціле r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ char *path = path_join(root, ETC_PASSWD_LOCK_PATH);
        якщо (!path)
                повернути log_oom_debug();

        (void) mkdir_parents(path, 0755);

        _cleanup_close_ ціле fd = open(path, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        якщо (fd < 0)
                повернути log_debug_errno(errno, "Cannot open %s: %m", path);

        r = unposix_lock(fd, LOCK_EX);
        якщо (r < 0)
                повернути log_debug_errno(r, "Locking %s failed: %m", path);

        повернути TAKE_FD(fd);
}

bool valid_user_group_name(const char *u, ValidUserFlags flags) {
        const char *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        якщо (isempty(u)) /* An empty user name is never valid */
                повернути false;

        якщо (parse_uid(u, NULL) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                повернути FLAGS_SET(flags, VALID_USER_ALLOW_NUMERIC);

        якщо (FLAGS_SET(flags, VALID_USER_RELAX)) {

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

                якщо (startswith(u, " ") || endswith(u, " ")) /* At least expect whitespace padding is removed
                                                             * at front and back (accept in the middle, since
                                                             * that's apparently a thing on Windows). Note
                                                             * that this also blocks usernames consisting of
                                                             * whitespace only. */
                        повернути false;

                якщо (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        повернути false;

                якщо (string_has_cc(u, NULL)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        повернути false;

                якщо (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        повернути false;

                якщо (in_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        повернути false;

                якщо (u[0] == '-' && in_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        повернути false;

                якщо (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        повернути false;

                /* Compare with strict result and warn if result doesn't match */
                якщо (FLAGS_SET(flags, VALID_USER_WARN) && !valid_user_group_name(u, 0))
                        log_struct(LOG_NOTICE,
                                   LOG_MESSAGE("Accepting user/group name '%s', which does not match strict user/group name rules.", u),
                                   LOG_ITEM("USER_GROUP_NAME=%s", u),
                                   LOG_MESSAGE_ID(SD_MESSAGE_UNSAFE_USER_NAME_STR));

                /* Note that we make no restrictions on the length in relaxed mode! */
        } інакше {
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

                якщо (!ascii_isalpha(u[0]) &&
                    u[0] != '_')
                        повернути false;

                для (i = u+1; *i; i++)
                        якщо (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !IN_SET(*i, '_', '-'))
                                повернути false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                якщо (l > (size_t) sz) /* glibc: 256 */
                        повернути false;
                якщо (l > NAME_MAX) /* must fit in a filename: 255 */
                        повернути false;
                якщо (l > sizeof_field(struct utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        повернути false;
        }

        повернути true;
}

bool valid_gecos(const char *d) {

        якщо (!d)
                повернути false;

        якщо (!utf8_is_valid(d))
                повернути false;

        якщо (string_has_cc(d, NULL))
                повернути false;

        /* Colons are used as field separators, and hence not OK */
        якщо (strchr(d, ':'))
                повернути false;

        повернути true;
}

char* mangle_gecos(const char *d) {
        char *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        якщо (!mangled)
                повернути NULL;

        для (char *i = mangled; *i; i++) {
                ціле len;

                якщо ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        continue;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                якщо (len < 0) {
                        *i = ' ';
                        continue;
                }

                i += len - 1;
        }

        повернути mangled;
}

bool valid_home(const char *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        якщо (isempty(p))
                повернути false;

        якщо (!utf8_is_valid(p))
                повернути false;

        якщо (string_has_cc(p, NULL))
                повернути false;

        якщо (!path_is_absolute(p))
                повернути false;

        якщо (!path_is_normalized(p))
                повернути false;

        /* Colons are used as field separators, and hence not OK */
        якщо (strchr(p, ':'))
                повернути false;

        повернути true;
}

bool valid_shell(const char *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        якщо (!valid_home(p))
                повернути false;

        повернути !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

ціле maybe_setgroups(size_t size, const gid_t *list) {
        ціле r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        якщо (size == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ FILE *f = fopen("/proc/self/gid_map", "re");
                якщо (!f && errno != ENOENT)
                        повернути -errno;
                якщо (f) {
                        r = safe_fgetc(f, /* ret= */ NULL);
                        якщо (r < 0)
                                повернути r;
                        якщо (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                повернути 0;
                        }
                }

                _cleanup_free_ char *setgroups_content = NULL;
                r = read_one_line_file("/proc/self/setgroups", &setgroups_content);
                якщо (r < 0 && r != -ENOENT)
                        повернути r;
                якщо (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        повернути 0;
                }
        }

        повернути RET_NERRNO(setgroups(size, list));
}

bool synthesize_nobody(void) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        static ціле cache = -1;

        якщо (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        повернути cache;
}

ціле putpwent_sane(const struct passwd *pw, FILE *stream) {
        assert(pw);
        assert(stream);

        errno = 0;
        якщо (putpwent(pw, stream) != 0)
                повернути errno_or_else(EIO);

        повернути 0;
}

ціле putspent_sane(const struct spwd *sp, FILE *stream) {
        assert(sp);
        assert(stream);

        errno = 0;
        якщо (putspent(sp, stream) != 0)
                повернути errno_or_else(EIO);

        повернути 0;
}

ціле putgrent_sane(const struct group *gr, FILE *stream) {
        assert(gr);
        assert(stream);

        errno = 0;
        якщо (putgrent(gr, stream) != 0)
                повернути errno_or_else(EIO);

        повернути 0;
}

#якщо ENABLE_GSHADOW
ціле putsgent_sane(const struct sgrp *sg, FILE *stream) {
        assert(sg);
        assert(stream);

        errno = 0;
        якщо (putsgent(sg, stream) != 0)
                повернути errno_or_else(EIO);

        повернути 0;
}
#endif

ціле fgetpwent_sane(FILE *stream, struct passwd **pw) {
        assert(stream);
        assert(pw);

        errno = 0;
        struct passwd *p = fgetpwent(stream);
        якщо (!p && !IN_SET(errno, 0, ENOENT))
                повернути -errno;

        *pw = p;
        повернути !!p;
}

ціле fgetspent_sane(FILE *stream, struct spwd **sp) {
        assert(stream);
        assert(sp);

        errno = 0;
        struct spwd *s = fgetspent(stream);
        якщо (!s && !IN_SET(errno, 0, ENOENT))
                повернути -errno;

        *sp = s;
        повернути !!s;
}

ціле fgetgrent_sane(FILE *stream, struct group **gr) {
        assert(stream);
        assert(gr);

        errno = 0;
        struct group *g = fgetgrent(stream);
        якщо (!g && !IN_SET(errno, 0, ENOENT))
                повернути -errno;

        *gr = g;
        повернути !!g;
}

#якщо ENABLE_GSHADOW
ціле fgetsgent_sane(FILE *stream, struct sgrp **sg) {
        assert(stream);
        assert(sg);

        errno = 0;
        struct sgrp *s = fgetsgent(stream);
        якщо (!s && !IN_SET(errno, 0, ENOENT))
                повернути -errno;

        *sg = s;
        повернути !!s;
}
#endif

ціле is_this_me(const char *username) {
        uid_t uid;
        ціле r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, NULL, &uid, NULL, NULL, NULL);
        якщо (r < 0)
                повернути r;

        повернути uid == getuid();
}

const char* get_home_root(void) {
        /* For debug purposes allow overriding where we look for home dirs */
        const char *e = secure_getenv("SYSTEMD_HOME_ROOT");
        якщо (e && path_is_absolute(e) && path_is_normalized(e))
                повернути e;

        повернути "/home";
}

static ціле copy_struct_passwd(const struct passwd *pw, struct passwd **ret) {
        assert(pw);
        assert(ret);

        size_t need_bytes = sizeof(struct passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        char *buf = malloc(need_bytes);
        якщо (!buf)
                повернути -ENOMEM;

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
        повернути 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
ціле lookup_pwent_in_files(
                char * const *files,
                const char *name,
                uid_t uid,
                struct passwd **ret) {

        ціле r;

        assert(files);
        assert(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = NULL;
                struct passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                якщо (r == -ENOENT)
                        continue;
                якщо (r < 0)
                        повернути r;

                поки ((r = fgetpwent_sane(f, &pw)) > 0) {
                        якщо (name && !streq_ptr(pw->pw_name, name))
                                continue;
                        якщо (uid_is_valid(uid) && pw->pw_uid != uid)
                                continue;
                        якщо (ret)
                                повернути copy_struct_passwd(pw, ret);
                        повернути 0;
                }
                якщо (r < 0)
                        повернути r;
        }

        повернути -ESRCH;
}

#якщо !BUILD_STATIC

static size_t getpw_buffer_size(void) {
        long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        повернути bufsize <= 0 ? 4096U : (size_t) bufsize;
}

static bool errno_is_user_doesnt_exist(ціле error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        повернути IN_SET(abs(error), ENOENT, ESRCH, EBADF, EPERM);
}

#endif

ціле getpwnam_malloc(const char *name, struct passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        якщо (isempty(name))
                повернути -EINVAL;

#якщо BUILD_STATIC
        повернути lookup_pwent_in_files(PASSWD_FILES, name, UID_INVALID, ret);
#інакше
        size_t bufsize = getpw_buffer_size();
        ціле r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        для (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(sizeof(struct passwd)));
                buf = malloc0(ALIGN(sizeof(struct passwd)) + bufsize);
                якщо (!buf)
                        повернути -ENOMEM;

                struct passwd *pw = NULL;
                r = getpwnam_r(name, buf, (char*) buf + ALIGN(sizeof(struct passwd)), bufsize, &pw);
                якщо (r == 0) {
                        якщо (pw) {
                                якщо (ret)
                                        *ret = TAKE_PTR(buf);
                                повернути 0;
                        }

                        повернути -ESRCH;
                }

                assert(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                якщо (errno_is_user_doesnt_exist(r))
                        повернути -ESRCH;
                якщо (r != ERANGE)
                        повернути -r;

                якщо (bufsize > SIZE_MAX/2 - ALIGN(sizeof(struct passwd)))
                        повернути -ENOMEM;
                bufsize *= 2;
        }
#endif
}

ціле getpwuid_malloc(uid_t uid, struct passwd **ret) {
        якщо (!uid_is_valid(uid))
                повернути -EINVAL;

#якщо BUILD_STATIC
        повернути lookup_pwent_in_files(PASSWD_FILES, /* name= */ NULL, uid, ret);
#інакше
        size_t bufsize = getpw_buffer_size();
        ціле r;

        для (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(sizeof(struct passwd)));
                buf = malloc0(ALIGN(sizeof(struct passwd)) + bufsize);
                якщо (!buf)
                        повернути -ENOMEM;

                struct passwd *pw = NULL;
                r = getpwuid_r(uid, buf, (char*) buf + ALIGN(sizeof(struct passwd)), bufsize, &pw);
                якщо (r == 0) {
                        якщо (pw) {
                                якщо (ret)
                                        *ret = TAKE_PTR(buf);
                                повернути 0;
                        }

                        повернути -ESRCH;
                }

                assert(r > 0);

                якщо (errno_is_user_doesnt_exist(r))
                        повернути -ESRCH;
                якщо (r != ERANGE)
                        повернути -r;

                якщо (bufsize > SIZE_MAX/2 - ALIGN(sizeof(struct passwd)))
                        повернути -ENOMEM;
                bufsize *= 2;
        }
#endif
}

static ціле copy_struct_group(const struct group *gr, struct group **ret) {
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
        якщо (!buf)
                повернути -ENOMEM;

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
        для (size_t i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = NULL;

        *ret = newgr;
        повернути 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
ціле lookup_grent_in_files(
                char * const *files,
                const char *name,
                gid_t gid,
                struct group **ret) {

        ціле r;

        assert(files);
        assert(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = NULL;
                struct group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                якщо (r == -ENOENT)
                        continue;
                якщо (r < 0)
                        повернути r;

                поки ((r = fgetgrent_sane(f, &gr)) > 0) {
                        якщо (name && !streq_ptr(gr->gr_name, name))
                                continue;
                        якщо (gid_is_valid(gid) && gr->gr_gid != gid)
                                continue;
                        якщо (ret)
                                повернути copy_struct_group(gr, ret);
                        повернути 0;
                }
                якщо (r < 0)
                        повернути r;
        }

        повернути -ESRCH;
}

ціле sysconf_ngroups_max(void) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        errno = 0;
        long ngroups_max = sysconf(_SC_NGROUPS_MAX);
        якщо (ngroups_max <= 0)
                повернути errno_or_else(EOPNOTSUPP);
        якщо (ngroups_max > INT_MAX)
                повернути -ERANGE;
        повернути ngroups_max;
}

ssize_t lookup_groups_in_files(
                char * const *files,
                const char *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = NULL;
        ssize_t n_arr = 0;
        ціле r;

        assert(files);
        assert(name);
        assert(gid_is_valid(gid));

        ціле ngroups_max = sysconf_ngroups_max();
        якщо (ngroups_max < 0)
                повернути ngroups_max;

        якщо (!GREEDY_REALLOC(arr, n_arr + 1))
                повернути -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = NULL;
                struct group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                якщо (r == -ENOENT)
                        continue;
                якщо (r < 0)
                        повернути r;

                поки ((r = fgetgrent_sane(f, &gr)) > 0) {
                        якщо (gid == gr->gr_gid)
                                continue;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        якщо (gid_list_has(arr, n_arr, gr->gr_gid))
                                continue;

                        якщо (!strv_contains(gr->gr_mem, name))
                                continue;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        якщо (n_arr >= ngroups_max)
                                повернути -ENOBUFS;

                        якщо (!GREEDY_REALLOC(arr, n_arr + 1))
                                повернути -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                якщо (r < 0)
                        повернути r;
        }

        якщо (ret)
                *ret = TAKE_PTR(arr);
        повернути n_arr;
}

ціле getgrouplist_malloc(const char *user, gid_t gid, gid_t **ret) {
#якщо BUILD_STATIC
        повернути lookup_groups_in_files(GROUP_FILES, user, gid, ret);
#інакше
        ціле ngroups_max = sysconf_ngroups_max();
        якщо (ngroups_max < 0)
                повернути ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        якщо (!gids)
                повернути -ENOMEM;

        ціле k = ngroups_max;
        якщо (getgrouplist(user, gid, gids, &k) < 0)
                повернути -EIO;  /* getgrouplist does not set errno */

        якщо (ret)
                *ret = TAKE_PTR(gids);
        повернути k;
#endif
}

ціле initgroups_wrapper(const char *user, gid_t gid) {
#якщо BUILD_STATIC
        _cleanup_free_ gid_t *groups = NULL;
        ціле r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        якщо (n_groups <= 0)
                повернути n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        для (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                якщо (r != -EINVAL)
                        break;
        }

        повернути r;
#інакше
        повернути RET_NERRNO(initgroups(user, gid));
#endif
}

#якщо !BUILD_STATIC

static size_t getgr_buffer_size(void) {
        long bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        повернути bufsize <= 0 ? 4096U : (size_t) bufsize;
}

#endif

ціле getgrnam_malloc(const char *name, struct group **ret) {
        якщо (isempty(name))
                повернути -EINVAL;

#якщо BUILD_STATIC
        повернути lookup_grent_in_files(GROUP_FILES, name, GID_INVALID, ret);
#інакше
        size_t bufsize = getgr_buffer_size();
        ціле r;

        для (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(sizeof(struct group)));
                buf = malloc0(ALIGN(sizeof(struct group)) + bufsize);
                якщо (!buf)
                        повернути -ENOMEM;

                struct group *gr = NULL;
                r = getgrnam_r(name, buf, (char*) buf + ALIGN(sizeof(struct group)), bufsize, &gr);
                якщо (r == 0) {
                        якщо (gr) {
                                якщо (ret)
                                        *ret = TAKE_PTR(buf);
                                повернути 0;
                        }

                        повернути -ESRCH;
                }

                assert(r > 0);

                якщо (errno_is_user_doesnt_exist(r))
                        повернути -ESRCH;
                якщо (r != ERANGE)
                        повернути -r;

                якщо (bufsize > SIZE_MAX/2 - ALIGN(sizeof(struct group)))
                        повернути -ENOMEM;
                bufsize *= 2;
        }
#endif
}

ціле getgrgid_malloc(gid_t gid, struct group **ret) {
        якщо (!gid_is_valid(gid))
                повернути -EINVAL;

#якщо BUILD_STATIC
        повернути lookup_grent_in_files(GROUP_FILES, /* name= */ NULL, gid, ret);
#інакше
        size_t bufsize = getgr_buffer_size();
        ціле r;

        для (;;) {
                _cleanup_free_ void *buf = NULL;

                /* Silence static analyzers */
                assert(bufsize <= SIZE_MAX - ALIGN(sizeof(struct group)));
                buf = malloc0(ALIGN(sizeof(struct group)) + bufsize);
                якщо (!buf)
                        повернути -ENOMEM;

                struct group *gr = NULL;
                r = getgrgid_r(gid, buf, (char*) buf + ALIGN(sizeof(struct group)), bufsize, &gr);
                якщо (r == 0) {
                        якщо (gr) {
                                якщо (ret)
                                        *ret = TAKE_PTR(buf);
                                повернути 0;
                        }

                        повернути -ESRCH;
                }

                assert(r > 0);

                якщо (errno_is_user_doesnt_exist(r))
                        повернути -ESRCH;
                якщо (r != ERANGE)
                        повернути -r;

                якщо (bufsize > SIZE_MAX/2 - ALIGN(sizeof(struct group)))
                        повернути -ENOMEM;
                bufsize *= 2;
        }
#endif
}
