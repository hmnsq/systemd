/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <sys/file.h>
#அந்தர்பாவ <sys/stat.h>
#அந்தர்பாவ <unistd.h>
#அந்தர்பாவ <utmpx.h>

#அந்தர்பாவ "sd-messages.h"

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "chase.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "extract-word.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "format-util.h"
#அந்தர்பாவ "lock-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "mkdir.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "terminal-util.h"
#அந்தர்பாவ "user-util.h"
#அந்தர்பாவ "utf8.h"

#define DEFINE_STRERROR_ACCOUNT(type)                                   \
        மாறிலி எழுத்து* strerror_##type(                                    \
                        முழு errnum,                                     \
                        எழுத்து *buf,                                      \
                        size_t buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                தேர்வு (errnum) {                                       \
                நிலை ESRCH:                                             \
                        திரும்பு "Unknown " STRINGIFY(type);              \
                நிலை ENOEXEC:                                           \
                        திரும்பு "Not a system " STRINGIFY(type);         \
                இயல்பு:                                                \
                        திரும்பு strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        எனில் (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                திரும்பு false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        எனில் (uid == (uid_t) UINT32_C(0xFFFF))
                திரும்பு false;

        திரும்பு true;
}

முழு parse_uid(மாறிலி எழுத்து *s, uid_t *ret) {
        uint32_t uid = 0;
        முழு r;

        ப்ரதிஜ்ஞா(s);

        assert_cc(அளவு(uid_t) == அளவு(uint32_t));

        /* We are very strict when parsing UIDs, and prohibit +/- as prefix, leading zero as prefix, and
         * whitespace. We do this, since this call is often used in a context where we parse things as UID
         * first, and if that doesn't work we fall back to NSS. Thus we really want to make sure that UIDs
         * are parsed as UIDs only if they really really look like UIDs. */
        r = safe_atou32_full(s, 10
                             | SAFE_ATO_REFUSE_PLUS_MINUS
                             | SAFE_ATO_REFUSE_LEADING_ZERO
                             | SAFE_ATO_REFUSE_LEADING_WHITESPACE, &uid);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (!uid_is_valid(uid))
                திரும்பு -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        எனில் (ret)
                *ret = uid;

        திரும்பு 0;
}

முழு parse_uid_range(மாறிலி எழுத்து *s, uid_t *ret_lower, uid_t *ret_upper) {
        _cleanup_free_ எழுத்து *word = வெற்று;
        uid_t l, u;
        முழு r;

        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(ret_lower);
        ப்ரதிஜ்ஞா(ret_upper);

        r = extract_first_word(&s, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == 0)
                திரும்பு -EINVAL;

        r = parse_uid(word, &l);
        எனில் (r < 0)
                திரும்பு r;

        /* Check for the upper bound and extract it if needed */
        எனில் (!s)
                /* Single number with no dash. */
                u = l;
        இல்லையெனில் எனில் (!*s)
                /* Trailing dash is an error. */
                திரும்பு -EINVAL;
        இல்லையெனில் {
                r = parse_uid(s, &u);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (l > u)
                        திரும்பு -EINVAL;
        }

        *ret_lower = l;
        *ret_upper = u;
        திரும்பு 0;
}

எழுத்து* getlogname_malloc(வெறுமை) {
        uid_t uid;
        கட்டமைப்பு stat st;

        எனில் (isatty_safe(STDIN_FILENO) && fstat(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        இல்லையெனில்
                uid = getuid();

        திரும்பு uid_to_name(uid);
}

எழுத்து* getusername_malloc(வெறுமை) {
        மாறிலி எழுத்து *e;

        e = secure_getenv("USER");
        எனில் (e)
                திரும்பு strdup(e);

        திரும்பு uid_to_name(getuid());
}

bool is_nologin_shell(மாறிலி எழுத்து *shell) {
        திரும்பு PATH_IN_SET(shell,
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

bool shell_is_placeholder(மாறிலி எழுத்து *shell) {
        திரும்பு isempty(shell) || is_nologin_shell(shell);
}

மாறிலி எழுத்து* default_வேர்_shell_at(முழு rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        ப்ரதிஜ்ஞா(rfd >= 0 || rfd == AT_FDCWD);

        முழு r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, வெற்று, வெற்று);
        எனில் (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        எனில் (r > 0)
                திரும்பு DEFAULT_USER_SHELL;

        திரும்பு "/bin/sh";
}

மாறிலி எழுத்து* default_வேர்_shell(மாறிலி எழுத்து *வேர்) {
        _cleanup_close_ முழு rfd = -EBADF;

        rfd = open(empty_to_வேர்(வேர்), O_CLOEXEC | O_DIRECTORY | O_PATH);
        எனில் (rfd < 0)
                திரும்பு "/bin/sh";

        திரும்பு default_வேர்_shell_at(rfd);
}

நிலையான முழு return_user_creds(
                மாறிலி எழுத்து *username,
                uid_t uid, gid_t gid,
                மாறிலி எழுத்து *home,
                மாறிலி எழுத்து *shell,
                எழுத்து **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                எழுத்து **ret_home,
                எழுத்து **ret_shell) {
        /* Helper function to help with the strdups and atomic setting of return params. */

        _cleanup_free_ எழுத்து *s1 = வெற்று, *s2 = வெற்று, *s3 = வெற்று;
        முழு r;

        எனில் (ret_username) {
                r = strdup_to(&s1, username);
                எனில் (r < 0)
                        திரும்பு r;
        }

        எனில் (ret_home) {
                r = strdup_to(&s2, home);
                எனில் (r < 0)
                        திரும்பு r;
        }

        எனில் (ret_shell) {
                r = strdup_to(&s3, shell);
                எனில் (r < 0)
                        திரும்பு r;
        }

        எனில் (ret_username)
                *ret_username = TAKE_PTR(s1);
        எனில் (ret_uid)
                *ret_uid = uid;
        எனில் (ret_gid)
                *ret_gid = gid;
        எனில் (ret_home)
                *ret_home = TAKE_PTR(s2);
        எனில் (ret_shell)
                *ret_shell = TAKE_PTR(s3);
        திரும்பு 0;
}

நிலையான முழு synthesize_user_creds(
                மாறிலி எழுத்து *username,
                UserCredsFlags flags,
                எழுத்து **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                எழுத்து **ret_home,
                எழுத்து **ret_shell) {
        ப்ரதிஜ்ஞா(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        எனில் (STR_IN_SET(username, "root", "0"))
                திரும்பு return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_வேர்_shell(வெற்று) : வெற்று,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        எனில் (STR_IN_SET(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                திரும்பு return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? வெற்று : "/",
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? வெற்று : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        திரும்பு -ENOMEDIUM;
}

முழு get_user_creds(
                மாறிலி எழுத்து *username,
                UserCredsFlags flags,
                எழுத்து **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                எழுத்து **ret_home,
                எழுத்து **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ கட்டமைப்பு passwd *pw = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(username);
        ப்ரதிஜ்ஞா((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        எனில் (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS) ||
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
                எனில் (r >= 0)
                        திரும்பு 0;
                எனில் (r != -ENOMEDIUM) /* not a username we can synthesize */
                        திரும்பு r;
        }

        எனில் (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                எனில் (r >= 0)
                        username = pw->pw_name;

                இல்லையெனில் எனில் (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        எனில் (ret_username)
                                *ret_username = வெற்று;
                        எனில் (ret_uid)
                                *ret_uid = u;
                        திரும்பு 0;
                }
        } இல்லையெனில்
                r = getpwnam_malloc(username, &pw);

        எனில் (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                எனில் (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        திரும்பு 0;

                திரும்பு r;
        }

        எனில் (ret_uid && !uid_is_valid(pw->pw_uid))
                திரும்பு -EBADMSG;

        எனில் (ret_gid && !gid_is_valid(pw->pw_gid))
                திரும்பு -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        மாறிலி எழுத்து *h =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && empty_or_வேர்(pw->pw_dir)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_dir) || !path_is_absolute(pw->pw_dir)))
                ? வெற்று : pw->pw_dir;

        மாறிலி எழுத்து *s =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_shell) || !path_is_absolute(pw->pw_shell)))
                ? வெற்று : pw->pw_shell;

        திரும்பு return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

நிலையான முழு synthesize_group_creds(மாறிலி எழுத்து *groupname, எழுத்து **ret_name, gid_t *ret_gid) {
        ப்ரதிஜ்ஞா(groupname);

        gid_t id;
        மாறிலி எழுத்து *n;
        முழு r;

        எனில் (STR_IN_SET(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } இல்லையெனில் எனில் (STR_IN_SET(groupname, NOBODY_GROUP_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_GROUP_NAME;
        } இல்லையெனில்
                திரும்பு -ENOMEDIUM;

        r = strdup_to_full(ret_name, n);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (ret_gid)
                *ret_gid = id;
        திரும்பு 0;
}

முழு get_group_creds(மாறிலி எழுத்து *groupname, UserCredsFlags flags, எழுத்து **ret_name, gid_t *ret_gid) {
        _cleanup_free_ கட்டமைப்பு group *gr = வெற்று;
        gid_t id;
        முழு r;

        ப்ரதிஜ்ஞா(groupname);

        எனில் (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_group_creds(groupname, ret_name, ret_gid);
                எனில் (r >= 0)
                        திரும்பு 0;
                எனில் (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        திரும்பு r;
        }

        எனில் (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                எனில் (r >= 0)
                        groupname = gr->gr_name;
                இல்லையெனில் எனில் (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING)) {
                        எனில் (ret_gid)
                                *ret_gid = id;
                        எனில் (ret_name)
                                *ret_name = வெற்று;
                        திரும்பு 0;
                }
        } இல்லையெனில்
                r = getgrnam_malloc(groupname, &gr);

        எனில் (r < 0) {
                எனில் (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_group_creds(groupname, ret_name, ret_gid) >= 0)
                        திரும்பு 0;
                திரும்பு r;
        }

        எனில் (ret_gid && !gid_is_valid(gr->gr_gid))
                திரும்பு -EBADMSG;

        r = strdup_to_full(ret_name, groupname);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (ret_gid)
                *ret_gid = gr->gr_gid;
        திரும்பு 0;
}

எழுத்து* uid_to_name(uid_t uid) {
        எழுத்து *ret;
        முழு r;

        /* Shortcut things to avoid NSS lookups */
        எனில் (uid == 0)
                திரும்பு strdup("root");
        எனில் (uid == UID_NOBODY && synthesize_nobody())
                திரும்பு strdup(NOBODY_USER_NAME);

        எனில் (uid_is_valid(uid)) {
                _cleanup_free_ கட்டமைப்பு passwd *pw = வெற்று;

                r = getpwuid_malloc(uid, &pw);
                எனில் (r >= 0)
                        திரும்பு strdup(pw->pw_name);
        }

        எனில் (asprintf(&ret, UID_FMT, uid) < 0)
                திரும்பு வெற்று;

        திரும்பு ret;
}

எழுத்து* gid_to_name(gid_t gid) {
        எழுத்து *ret;
        முழு r;

        எனில் (gid == 0)
                திரும்பு strdup("root");
        எனில் (gid == GID_NOBODY && synthesize_nobody())
                திரும்பு strdup(NOBODY_GROUP_NAME);

        எனில் (gid_is_valid(gid)) {
                _cleanup_free_ கட்டமைப்பு group *gr = வெற்று;

                r = getgrgid_malloc(gid, &gr);
                எனில் (r >= 0)
                        திரும்பு strdup(gr->gr_name);
        }

        எனில் (asprintf(&ret, GID_FMT, gid) < 0)
                திரும்பு வெற்று;

        திரும்பு ret;
}

நிலையான bool gid_list_has(மாறிலி gid_t *list, size_t size, gid_t val) {
        ப்ரதிஜ்ஞா(list || size == 0);

        FOREACH_ARRAY(i, list, size)
                எனில் (*i == val)
                        திரும்பு true;

        திரும்பு false;
}

முழு in_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = வெற்று;
        முழு ngroups;

        எனில் (getgid() == gid)
                திரும்பு 1;

        எனில் (getegid() == gid)
                திரும்பு 1;

        எனில் (!gid_is_valid(gid))
                திரும்பு -EINVAL;

        ngroups = getgroups_alloc(&gids);
        எனில் (ngroups < 0)
                திரும்பு ngroups;

        திரும்பு gid_list_has(gids, ngroups, gid);
}

முழு in_group(மாறிலி எழுத்து *name) {
        முழு r;
        gid_t gid;

        r = get_group_creds(name, /* flags= */ 0, /* ret_name= */ வெற்று, &gid);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு in_gid(gid);
}

முழு merge_gid_lists(மாறிலி gid_t *list1, size_t size1, மாறிலி gid_t *list2, size_t size2, gid_t **ret) {
        size_t nresult = 0;

        ப்ரதிஜ்ஞா(size1 == 0 || list1);
        ப்ரதிஜ்ஞா(size2 == 0 || list2);
        ப்ரதிஜ்ஞா(ret);

        எனில் (size2 > INT_MAX - size1)
                திரும்பு -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        எனில் (!buf)
                திரும்பு -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        க்கு (size_t i = 0; i < size1; i++)
                எனில் (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        க்கு (size_t i = 0; i < size2; i++)
                எனில் (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        திரும்பு (முழு)nresult;
}

முழு getgroups_alloc(gid_t **ret) {
        முழு ngroups = 8;

        ப்ரதிஜ்ஞா(ret);

        க்கு (unsigned attempt = 0;;) {
                _cleanup_free_ gid_t *p = வெற்று;

                p = new(gid_t, ngroups);
                எனில் (!p)
                        திரும்பு -ENOMEM;

                ngroups = getgroups(ngroups, p);
                எனில் (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        திரும்பு ngroups;
                }
                எனில் (ngroups == 0)
                        நிறுத்து;
                எனில் (errno != EINVAL)
                        திரும்பு -errno;

                /* Give up eventually */
                எனில் (attempt++ > 10)
                        திரும்பு -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, வெற்று);
                எனில் (ngroups < 0)
                        திரும்பு -errno;
                எனில் (ngroups == 0)
                        நிறுத்து;
        }

        *ret = வெற்று;
        திரும்பு 0;
}

முழு get_home_dir(எழுத்து **ret) {
        _cleanup_free_ கட்டமைப்பு passwd *p = வெற்று;
        மாறிலி எழுத்து *e;
        uid_t u;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        எனில் (e && path_is_valid(e) && path_is_absolute(e))
                செல் found;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        எனில் (u == 0) {
                e = "/root";
                செல் found;
        }
        எனில் (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                செல் found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        எனில் (r < 0)
                திரும்பு r;

        e = p->pw_dir;
        எனில் (!path_is_valid(e) || !path_is_absolute(e))
                திரும்பு -EINVAL;

 found:
        திரும்பு path_simplify_alloc(e, ret);
}

முழு get_shell(எழுத்து **ret) {
        _cleanup_free_ கட்டமைப்பு passwd *p = வெற்று;
        மாறிலி எழுத்து *e;
        uid_t u;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        எனில் (e && path_is_valid(e) && path_is_absolute(e))
                செல் found;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        எனில் (u == 0) {
                e = default_வேர்_shell(வெற்று);
                செல் found;
        }
        எனில் (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                செல் found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        எனில் (r < 0)
                திரும்பு r;

        e = p->pw_shell;
        எனில் (!path_is_valid(e) || !path_is_absolute(e))
                திரும்பு -EINVAL;

 found:
        திரும்பு path_simplify_alloc(e, ret);
}

முழு fully_set_uid_gid(uid_t uid, gid_t gid, மாறிலி gid_t supplementary_gids[], size_t n_supplementary_gids) {
        முழு r;

        ப்ரதிஜ்ஞா(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (gid_is_valid(gid))
                எனில் (setresgid(gid, gid, gid) < 0)
                        திரும்பு -errno;

        எனில் (uid_is_valid(uid))
                எனில் (setresuid(uid, uid, uid) < 0)
                        திரும்பு -errno;

        திரும்பு 0;
}

முழு take_etc_passwd_lock(மாறிலி எழுத்து *வேர்) {
        முழு r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ எழுத்து *path = path_join(வேர், ETC_PASSWD_LOCK_PATH);
        எனில் (!path)
                திரும்பு log_oom_debug();

        (வெறுமை) mkdir_parents(path, 0755);

        _cleanup_close_ முழு fd = open(path, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        எனில் (fd < 0)
                திரும்பு log_debug_errno(errno, "Cannot open %s: %m", path);

        r = unposix_lock(fd, LOCK_EX);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Locking %s failed: %m", path);

        திரும்பு TAKE_FD(fd);
}

bool valid_user_group_name(மாறிலி எழுத்து *u, ValidUserFlags flags) {
        மாறிலி எழுத்து *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        எனில் (isempty(u)) /* An empty user name is never valid */
                திரும்பு false;

        எனில் (parse_uid(u, வெற்று) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                திரும்பு FLAGS_SET(flags, VALID_USER_ALLOW_NUMERIC);

        எனில் (FLAGS_SET(flags, VALID_USER_RELAX)) {

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

                எனில் (startswith(u, " ") || endswith(u, " ")) /* At least expect whitespace padding is removed
                                                             * at front and back (accept in the middle, since
                                                             * that's apparently a thing on Windows). Note
                                                             * that this also blocks usernames consisting of
                                                             * whitespace only. */
                        திரும்பு false;

                எனில் (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        திரும்பு false;

                எனில் (string_has_cc(u, வெற்று)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        திரும்பு false;

                எனில் (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        திரும்பு false;

                எனில் (in_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        திரும்பு false;

                எனில் (u[0] == '-' && in_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        திரும்பு false;

                எனில் (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        திரும்பு false;

                /* Compare with strict result and warn if result doesn't match */
                எனில் (FLAGS_SET(flags, VALID_USER_WARN) && !valid_user_group_name(u, 0))
                        log_struct(LOG_NOTICE,
                                   LOG_MESSAGE("Accepting user/group name '%s', which does not match strict user/group name rules.", u),
                                   LOG_ITEM("USER_GROUP_NAME=%s", u),
                                   LOG_MESSAGE_ID(SD_MESSAGE_UNSAFE_USER_NAME_STR));

                /* Note that we make no restrictions on the length in relaxed mode! */
        } இல்லையெனில் {
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

                எனில் (!ascii_isalpha(u[0]) &&
                    u[0] != '_')
                        திரும்பு false;

                க்கு (i = u+1; *i; i++)
                        எனில் (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !IN_SET(*i, '_', '-'))
                                திரும்பு false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                எனில் (l > (size_t) sz) /* glibc: 256 */
                        திரும்பு false;
                எனில் (l > NAME_MAX) /* must fit in a filename: 255 */
                        திரும்பு false;
                எனில் (l > sizeof_field(கட்டமைப்பு utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        திரும்பு false;
        }

        திரும்பு true;
}

bool valid_gecos(மாறிலி எழுத்து *d) {

        எனில் (!d)
                திரும்பு false;

        எனில் (!utf8_is_valid(d))
                திரும்பு false;

        எனில் (string_has_cc(d, வெற்று))
                திரும்பு false;

        /* Colons are used as field separators, and hence not OK */
        எனில் (strchr(d, ':'))
                திரும்பு false;

        திரும்பு true;
}

எழுத்து* mangle_gecos(மாறிலி எழுத்து *d) {
        எழுத்து *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        எனில் (!mangled)
                திரும்பு வெற்று;

        க்கு (எழுத்து *i = mangled; *i; i++) {
                முழு len;

                எனில் ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        தொடர்;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                எனில் (len < 0) {
                        *i = ' ';
                        தொடர்;
                }

                i += len - 1;
        }

        திரும்பு mangled;
}

bool valid_home(மாறிலி எழுத்து *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        எனில் (isempty(p))
                திரும்பு false;

        எனில் (!utf8_is_valid(p))
                திரும்பு false;

        எனில் (string_has_cc(p, வெற்று))
                திரும்பு false;

        எனில் (!path_is_absolute(p))
                திரும்பு false;

        எனில் (!path_is_normalized(p))
                திரும்பு false;

        /* Colons are used as field separators, and hence not OK */
        எனில் (strchr(p, ':'))
                திரும்பு false;

        திரும்பு true;
}

bool valid_shell(மாறிலி எழுத்து *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        எனில் (!valid_home(p))
                திரும்பு false;

        திரும்பு !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

முழு maybe_setgroups(size_t size, மாறிலி gid_t *list) {
        முழு r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        எனில் (size == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ FILE *f = fopen("/proc/self/gid_map", "re");
                எனில் (!f && errno != ENOENT)
                        திரும்பு -errno;
                எனில் (f) {
                        r = safe_fgetc(f, /* ret= */ வெற்று);
                        எனில் (r < 0)
                                திரும்பு r;
                        எனில் (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                திரும்பு 0;
                        }
                }

                _cleanup_free_ எழுத்து *setgroups_content = வெற்று;
                r = read_one_வரி_file("/proc/self/setgroups", &setgroups_content);
                எனில் (r < 0 && r != -ENOENT)
                        திரும்பு r;
                எனில் (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        திரும்பு 0;
                }
        }

        திரும்பு RET_NERRNO(setgroups(size, list));
}

bool synthesize_nobody(வெறுமை) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        நிலையான முழு cache = -1;

        எனில் (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        திரும்பு cache;
}

முழு putpwent_sane(மாறிலி கட்டமைப்பு passwd *pw, FILE *stream) {
        ப்ரதிஜ்ஞா(pw);
        ப்ரதிஜ்ஞா(stream);

        errno = 0;
        எனில் (putpwent(pw, stream) != 0)
                திரும்பு errno_or_else(EIO);

        திரும்பு 0;
}

முழு putspent_sane(மாறிலி கட்டமைப்பு spwd *sp, FILE *stream) {
        ப்ரதிஜ்ஞா(sp);
        ப்ரதிஜ்ஞா(stream);

        errno = 0;
        எனில் (putspent(sp, stream) != 0)
                திரும்பு errno_or_else(EIO);

        திரும்பு 0;
}

முழு putgrent_sane(மாறிலி கட்டமைப்பு group *gr, FILE *stream) {
        ப்ரதிஜ்ஞா(gr);
        ப்ரதிஜ்ஞா(stream);

        errno = 0;
        எனில் (putgrent(gr, stream) != 0)
                திரும்பு errno_or_else(EIO);

        திரும்பு 0;
}

#எனில் ENABLE_GSHADOW
முழு putsgent_sane(மாறிலி கட்டமைப்பு sgrp *sg, FILE *stream) {
        ப்ரதிஜ்ஞா(sg);
        ப்ரதிஜ்ஞா(stream);

        errno = 0;
        எனில் (putsgent(sg, stream) != 0)
                திரும்பு errno_or_else(EIO);

        திரும்பு 0;
}
#endif

முழு fgetpwent_sane(FILE *stream, கட்டமைப்பு passwd **pw) {
        ப்ரதிஜ்ஞா(stream);
        ப்ரதிஜ்ஞா(pw);

        errno = 0;
        கட்டமைப்பு passwd *p = fgetpwent(stream);
        எனில் (!p && !IN_SET(errno, 0, ENOENT))
                திரும்பு -errno;

        *pw = p;
        திரும்பு !!p;
}

முழு fgetspent_sane(FILE *stream, கட்டமைப்பு spwd **sp) {
        ப்ரதிஜ்ஞா(stream);
        ப்ரதிஜ்ஞா(sp);

        errno = 0;
        கட்டமைப்பு spwd *s = fgetspent(stream);
        எனில் (!s && !IN_SET(errno, 0, ENOENT))
                திரும்பு -errno;

        *sp = s;
        திரும்பு !!s;
}

முழு fgetgrent_sane(FILE *stream, கட்டமைப்பு group **gr) {
        ப்ரதிஜ்ஞா(stream);
        ப்ரதிஜ்ஞா(gr);

        errno = 0;
        கட்டமைப்பு group *g = fgetgrent(stream);
        எனில் (!g && !IN_SET(errno, 0, ENOENT))
                திரும்பு -errno;

        *gr = g;
        திரும்பு !!g;
}

#எனில் ENABLE_GSHADOW
முழு fgetsgent_sane(FILE *stream, கட்டமைப்பு sgrp **sg) {
        ப்ரதிஜ்ஞா(stream);
        ப்ரதிஜ்ஞா(sg);

        errno = 0;
        கட்டமைப்பு sgrp *s = fgetsgent(stream);
        எனில் (!s && !IN_SET(errno, 0, ENOENT))
                திரும்பு -errno;

        *sg = s;
        திரும்பு !!s;
}
#endif

முழு is_this_me(மாறிலி எழுத்து *username) {
        uid_t uid;
        முழு r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, வெற்று, &uid, வெற்று, வெற்று, வெற்று);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு uid == getuid();
}

மாறிலி எழுத்து* get_home_வேர்(வெறுமை) {
        /* For debug purposes allow overriding where we look for home dirs */
        மாறிலி எழுத்து *e = secure_getenv("SYSTEMD_HOME_ROOT");
        எனில் (e && path_is_absolute(e) && path_is_normalized(e))
                திரும்பு e;

        திரும்பு "/home";
}

நிலையான முழு copy_struct_passwd(மாறிலி கட்டமைப்பு passwd *pw, கட்டமைப்பு passwd **ret) {
        ப்ரதிஜ்ஞா(pw);
        ப்ரதிஜ்ஞா(ret);

        size_t need_bytes = அளவு(கட்டமைப்பு passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        எழுத்து *buf = malloc(need_bytes);
        எனில் (!buf)
                திரும்பு -ENOMEM;

        கட்டமைப்பு passwd *newpw = (கட்டமைப்பு passwd *) buf;

        /* The layout in our buffer:
         * struct passwd, and then individual strings. */
        எழுத்து *p = buf + அளவு(கட்டமைப்பு passwd);

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
        திரும்பு 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
முழு lookup_pwent_in_files(
                எழுத்து * மாறிலி *files,
                மாறிலி எழுத்து *name,
                uid_t uid,
                கட்டமைப்பு passwd **ret) {

        முழு r;

        ப்ரதிஜ்ஞா(files);
        ப்ரதிஜ்ஞா(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = வெற்று;
                கட்டமைப்பு passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                எனில் (r == -ENOENT)
                        தொடர்;
                எனில் (r < 0)
                        திரும்பு r;

                போது ((r = fgetpwent_sane(f, &pw)) > 0) {
                        எனில் (name && !streq_ptr(pw->pw_name, name))
                                தொடர்;
                        எனில் (uid_is_valid(uid) && pw->pw_uid != uid)
                                தொடர்;
                        எனில் (ret)
                                திரும்பு copy_struct_passwd(pw, ret);
                        திரும்பு 0;
                }
                எனில் (r < 0)
                        திரும்பு r;
        }

        திரும்பு -ESRCH;
}

#எனில் !BUILD_STATIC

நிலையான size_t getpw_buffer_size(வெறுமை) {
        long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        திரும்பு bufsize <= 0 ? 4096U : (size_t) bufsize;
}

நிலையான bool errno_is_user_doesnt_exist(முழு error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        திரும்பு IN_SET(abs(error), ENOENT, ESRCH, EBADF, EPERM);
}

#endif

முழு getpwnam_malloc(மாறிலி எழுத்து *name, கட்டமைப்பு passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        எனில் (isempty(name))
                திரும்பு -EINVAL;

#எனில் BUILD_STATIC
        திரும்பு lookup_pwent_in_files(PASSWD_FILES, name, UID_INVALID, ret);
#இல்லையெனில்
        size_t bufsize = getpw_buffer_size();
        முழு r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        க்கு (;;) {
                _cleanup_free_ வெறுமை *buf = வெற்று;

                /* Silence static analyzers */
                ப்ரதிஜ்ஞா(bufsize <= SIZE_MAX - ALIGN(அளவு(கட்டமைப்பு passwd)));
                buf = malloc0(ALIGN(அளவு(கட்டமைப்பு passwd)) + bufsize);
                எனில் (!buf)
                        திரும்பு -ENOMEM;

                கட்டமைப்பு passwd *pw = வெற்று;
                r = getpwnam_r(name, buf, (எழுத்து*) buf + ALIGN(அளவு(கட்டமைப்பு passwd)), bufsize, &pw);
                எனில் (r == 0) {
                        எனில் (pw) {
                                எனில் (ret)
                                        *ret = TAKE_PTR(buf);
                                திரும்பு 0;
                        }

                        திரும்பு -ESRCH;
                }

                ப்ரதிஜ்ஞா(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                எனில் (errno_is_user_doesnt_exist(r))
                        திரும்பு -ESRCH;
                எனில் (r != ERANGE)
                        திரும்பு -r;

                எனில் (bufsize > SIZE_MAX/2 - ALIGN(அளவு(கட்டமைப்பு passwd)))
                        திரும்பு -ENOMEM;
                bufsize *= 2;
        }
#endif
}

முழு getpwuid_malloc(uid_t uid, கட்டமைப்பு passwd **ret) {
        எனில் (!uid_is_valid(uid))
                திரும்பு -EINVAL;

#எனில் BUILD_STATIC
        திரும்பு lookup_pwent_in_files(PASSWD_FILES, /* name= */ வெற்று, uid, ret);
#இல்லையெனில்
        size_t bufsize = getpw_buffer_size();
        முழு r;

        க்கு (;;) {
                _cleanup_free_ வெறுமை *buf = வெற்று;

                /* Silence static analyzers */
                ப்ரதிஜ்ஞா(bufsize <= SIZE_MAX - ALIGN(அளவு(கட்டமைப்பு passwd)));
                buf = malloc0(ALIGN(அளவு(கட்டமைப்பு passwd)) + bufsize);
                எனில் (!buf)
                        திரும்பு -ENOMEM;

                கட்டமைப்பு passwd *pw = வெற்று;
                r = getpwuid_r(uid, buf, (எழுத்து*) buf + ALIGN(அளவு(கட்டமைப்பு passwd)), bufsize, &pw);
                எனில் (r == 0) {
                        எனில் (pw) {
                                எனில் (ret)
                                        *ret = TAKE_PTR(buf);
                                திரும்பு 0;
                        }

                        திரும்பு -ESRCH;
                }

                ப்ரதிஜ்ஞா(r > 0);

                எனில் (errno_is_user_doesnt_exist(r))
                        திரும்பு -ESRCH;
                எனில் (r != ERANGE)
                        திரும்பு -r;

                எனில் (bufsize > SIZE_MAX/2 - ALIGN(அளவு(கட்டமைப்பு passwd)))
                        திரும்பு -ENOMEM;
                bufsize *= 2;
        }
#endif
}

நிலையான முழு copy_struct_group(மாறிலி கட்டமைப்பு group *gr, கட்டமைப்பு group **ret) {
        ப்ரதிஜ்ஞா(gr);
        ப்ரதிஜ்ஞா(ret);

        size_t need_bytes = அளவு(கட்டமைப்பு group)
                + strlen_ptr(gr->gr_name) + 1
                + strlen_ptr(gr->gr_passwd) + 1,
                n_mem = 0;
        STRV_FOREACH(s, gr->gr_mem) {
                need_bytes += அளவு(எழுத்து*) + strlen(*s) + 1;
                n_mem++;
        }
        need_bytes += அளவு(எழுத்து*);  /* NULL terminator for gr_mem */

        எழுத்து *buf = malloc(need_bytes);
        எனில் (!buf)
                திரும்பு -ENOMEM;

        கட்டமைப்பு group *newgr = (கட்டமைப்பு group *) buf;

        /* The layout in our buffer:
         * struct group, ->gr_mem pointers terminated by NULL, ->gr_name, ->gr_passwd, ->gr_mem items */
        /* The ->gr_mem array is first, because it needs alignment. */
        assert_cc(அளவு(கட்டமைப்பு group) % alignof(எழுத்து*) == 0);

        எழுத்து *p = buf + அளவு(கட்டமைப்பு group) + (n_mem + 1) * அளவு(எழுத்து*);

        newgr->gr_name = p;
        p = stpcpy(p, strempty(gr->gr_name)) + 1;

        newgr->gr_passwd = p;
        p = stpcpy(p, strempty(gr->gr_passwd)) + 1;

        newgr->gr_gid = gr->gr_gid;

        newgr->gr_mem = (எழுத்து**) (buf + அளவு(கட்டமைப்பு group));
        க்கு (size_t i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = வெற்று;

        *ret = newgr;
        திரும்பு 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
முழு lookup_grent_in_files(
                எழுத்து * மாறிலி *files,
                மாறிலி எழுத்து *name,
                gid_t gid,
                கட்டமைப்பு group **ret) {

        முழு r;

        ப்ரதிஜ்ஞா(files);
        ப்ரதிஜ்ஞா(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = வெற்று;
                கட்டமைப்பு group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                எனில் (r == -ENOENT)
                        தொடர்;
                எனில் (r < 0)
                        திரும்பு r;

                போது ((r = fgetgrent_sane(f, &gr)) > 0) {
                        எனில் (name && !streq_ptr(gr->gr_name, name))
                                தொடர்;
                        எனில் (gid_is_valid(gid) && gr->gr_gid != gid)
                                தொடர்;
                        எனில் (ret)
                                திரும்பு copy_struct_group(gr, ret);
                        திரும்பு 0;
                }
                எனில் (r < 0)
                        திரும்பு r;
        }

        திரும்பு -ESRCH;
}

முழு sysconf_ngroups_max(வெறுமை) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        errno = 0;
        long ngroups_max = sysconf(_SC_NGROUPS_MAX);
        எனில் (ngroups_max <= 0)
                திரும்பு errno_or_else(EOPNOTSUPP);
        எனில் (ngroups_max > INT_MAX)
                திரும்பு -ERANGE;
        திரும்பு ngroups_max;
}

ssize_t lookup_groups_in_files(
                எழுத்து * மாறிலி *files,
                மாறிலி எழுத்து *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = வெற்று;
        ssize_t n_arr = 0;
        முழு r;

        ப்ரதிஜ்ஞா(files);
        ப்ரதிஜ்ஞா(name);
        ப்ரதிஜ்ஞா(gid_is_valid(gid));

        முழு ngroups_max = sysconf_ngroups_max();
        எனில் (ngroups_max < 0)
                திரும்பு ngroups_max;

        எனில் (!GREEDY_REALLOC(arr, n_arr + 1))
                திரும்பு -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = வெற்று;
                கட்டமைப்பு group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                எனில் (r == -ENOENT)
                        தொடர்;
                எனில் (r < 0)
                        திரும்பு r;

                போது ((r = fgetgrent_sane(f, &gr)) > 0) {
                        எனில் (gid == gr->gr_gid)
                                தொடர்;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        எனில் (gid_list_has(arr, n_arr, gr->gr_gid))
                                தொடர்;

                        எனில் (!strv_contains(gr->gr_mem, name))
                                தொடர்;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        எனில் (n_arr >= ngroups_max)
                                திரும்பு -ENOBUFS;

                        எனில் (!GREEDY_REALLOC(arr, n_arr + 1))
                                திரும்பு -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                எனில் (r < 0)
                        திரும்பு r;
        }

        எனில் (ret)
                *ret = TAKE_PTR(arr);
        திரும்பு n_arr;
}

முழு getgrouplist_malloc(மாறிலி எழுத்து *user, gid_t gid, gid_t **ret) {
#எனில் BUILD_STATIC
        திரும்பு lookup_groups_in_files(GROUP_FILES, user, gid, ret);
#இல்லையெனில்
        முழு ngroups_max = sysconf_ngroups_max();
        எனில் (ngroups_max < 0)
                திரும்பு ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        எனில் (!gids)
                திரும்பு -ENOMEM;

        முழு k = ngroups_max;
        எனில் (getgrouplist(user, gid, gids, &k) < 0)
                திரும்பு -EIO;  /* getgrouplist does not set errno */

        எனில் (ret)
                *ret = TAKE_PTR(gids);
        திரும்பு k;
#endif
}

முழு initgroups_wrapper(மாறிலி எழுத்து *user, gid_t gid) {
#எனில் BUILD_STATIC
        _cleanup_free_ gid_t *groups = வெற்று;
        முழு r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        எனில் (n_groups <= 0)
                திரும்பு n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        க்கு (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                எனில் (r != -EINVAL)
                        நிறுத்து;
        }

        திரும்பு r;
#இல்லையெனில்
        திரும்பு RET_NERRNO(initgroups(user, gid));
#endif
}

#எனில் !BUILD_STATIC

நிலையான size_t getgr_buffer_size(வெறுமை) {
        long bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        திரும்பு bufsize <= 0 ? 4096U : (size_t) bufsize;
}

#endif

முழு getgrnam_malloc(மாறிலி எழுத்து *name, கட்டமைப்பு group **ret) {
        எனில் (isempty(name))
                திரும்பு -EINVAL;

#எனில் BUILD_STATIC
        திரும்பு lookup_grent_in_files(GROUP_FILES, name, GID_INVALID, ret);
#இல்லையெனில்
        size_t bufsize = getgr_buffer_size();
        முழு r;

        க்கு (;;) {
                _cleanup_free_ வெறுமை *buf = வெற்று;

                /* Silence static analyzers */
                ப்ரதிஜ்ஞா(bufsize <= SIZE_MAX - ALIGN(அளவு(கட்டமைப்பு group)));
                buf = malloc0(ALIGN(அளவு(கட்டமைப்பு group)) + bufsize);
                எனில் (!buf)
                        திரும்பு -ENOMEM;

                கட்டமைப்பு group *gr = வெற்று;
                r = getgrnam_r(name, buf, (எழுத்து*) buf + ALIGN(அளவு(கட்டமைப்பு group)), bufsize, &gr);
                எனில் (r == 0) {
                        எனில் (gr) {
                                எனில் (ret)
                                        *ret = TAKE_PTR(buf);
                                திரும்பு 0;
                        }

                        திரும்பு -ESRCH;
                }

                ப்ரதிஜ்ஞா(r > 0);

                எனில் (errno_is_user_doesnt_exist(r))
                        திரும்பு -ESRCH;
                எனில் (r != ERANGE)
                        திரும்பு -r;

                எனில் (bufsize > SIZE_MAX/2 - ALIGN(அளவு(கட்டமைப்பு group)))
                        திரும்பு -ENOMEM;
                bufsize *= 2;
        }
#endif
}

முழு getgrgid_malloc(gid_t gid, கட்டமைப்பு group **ret) {
        எனில் (!gid_is_valid(gid))
                திரும்பு -EINVAL;

#எனில் BUILD_STATIC
        திரும்பு lookup_grent_in_files(GROUP_FILES, /* name= */ வெற்று, gid, ret);
#இல்லையெனில்
        size_t bufsize = getgr_buffer_size();
        முழு r;

        க்கு (;;) {
                _cleanup_free_ வெறுமை *buf = வெற்று;

                /* Silence static analyzers */
                ப்ரதிஜ்ஞா(bufsize <= SIZE_MAX - ALIGN(அளவு(கட்டமைப்பு group)));
                buf = malloc0(ALIGN(அளவு(கட்டமைப்பு group)) + bufsize);
                எனில் (!buf)
                        திரும்பு -ENOMEM;

                கட்டமைப்பு group *gr = வெற்று;
                r = getgrgid_r(gid, buf, (எழுத்து*) buf + ALIGN(அளவு(கட்டமைப்பு group)), bufsize, &gr);
                எனில் (r == 0) {
                        எனில் (gr) {
                                எனில் (ret)
                                        *ret = TAKE_PTR(buf);
                                திரும்பு 0;
                        }

                        திரும்பு -ESRCH;
                }

                ப்ரதிஜ்ஞா(r > 0);

                எனில் (errno_is_user_doesnt_exist(r))
                        திரும்பு -ESRCH;
                எனில் (r != ERANGE)
                        திரும்பு -r;

                எனில் (bufsize > SIZE_MAX/2 - ALIGN(அளவு(கட்டமைப்பு group)))
                        திரும்பு -ENOMEM;
                bufsize *= 2;
        }
#endif
}
