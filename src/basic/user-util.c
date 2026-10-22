/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <sys/file.h>
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <unistd.h>
#अन्तर्भाव <utmpx.h>

#अन्तर्भाव "sd-messages.h"

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "chase.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "format-util.h"
#अन्तर्भाव "lock-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "mkdir.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "terminal-util.h"
#अन्तर्भाव "user-util.h"
#अन्तर्भाव "utf8.h"

#define DEFINE_STRERROR_ACCOUNT(type)                                   \
        स्थिरांक अक्षर* strerror_##type(                                    \
                        पूर्णांक errnum,                                     \
                        अक्षर *buf,                                      \
                        माप_प्रकार buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                निवड (errnum) {                                       \
                स्थिती ESRCH:                                             \
                        परत "Unknown " STRINGIFY(type);              \
                स्थिती ENOEXEC:                                           \
                        परत "Not a system " STRINGIFY(type);         \
                पूर्वनिर्धारित:                                                \
                        परत strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        जर (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                परत false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        जर (uid == (uid_t) UINT32_C(0xFFFF))
                परत false;

        परत true;
}

पूर्णांक parse_uid(स्थिरांक अक्षर *s, uid_t *ret) {
        uint32_t uid = 0;
        पूर्णांक r;

        प्रतिज्ञा(s);

        assert_cc(आकार(uid_t) == आकार(uint32_t));

        /* We are very strict when parsing UIDs, and prohibit +/- as prefix, leading zero as prefix, and
         * whitespace. We do this, since this call is often used in a context where we parse things as UID
         * first, and if that doesn't work we fall back to NSS. Thus we really want to make sure that UIDs
         * are parsed as UIDs only if they really really look like UIDs. */
        r = safe_atou32_full(s, 10
                             | SAFE_ATO_REFUSE_PLUS_MINUS
                             | SAFE_ATO_REFUSE_LEADING_ZERO
                             | SAFE_ATO_REFUSE_LEADING_WHITESPACE, &uid);
        जर (r < 0)
                परत r;

        जर (!uid_is_valid(uid))
                परत -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        जर (ret)
                *ret = uid;

        परत 0;
}

पूर्णांक parse_uid_range(स्थिरांक अक्षर *s, uid_t *ret_lower, uid_t *ret_upper) {
        _cleanup_free_ अक्षर *word = रिक्त;
        uid_t l, u;
        पूर्णांक r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret_lower);
        प्रतिज्ञा(ret_upper);

        r = extract_first_word(&s, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        जर (r < 0)
                परत r;
        जर (r == 0)
                परत -EINVAL;

        r = parse_uid(word, &l);
        जर (r < 0)
                परत r;

        /* Check for the upper bound and extract it if needed */
        जर (!s)
                /* Single number with no dash. */
                u = l;
        अन्यथा जर (!*s)
                /* Trailing dash is an error. */
                परत -EINVAL;
        अन्यथा {
                r = parse_uid(s, &u);
                जर (r < 0)
                        परत r;

                जर (l > u)
                        परत -EINVAL;
        }

        *ret_lower = l;
        *ret_upper = u;
        परत 0;
}

अक्षर* getlogname_malloc(शून्य) {
        uid_t uid;
        संरचना stat st;

        जर (isatty_safe(STDIN_FILENO) && ख_स्थिति(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        अन्यथा
                uid = getuid();

        परत uid_to_name(uid);
}

अक्षर* getusername_malloc(शून्य) {
        स्थिरांक अक्षर *e;

        e = secure_getenv("USER");
        जर (e)
                परत strdup(e);

        परत uid_to_name(getuid());
}

bool is_nologin_shell(स्थिरांक अक्षर *shell) {
        परत PATH_IN_SET(shell,
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

bool shell_is_placeholder(स्थिरांक अक्षर *shell) {
        परत isempty(shell) || is_nologin_shell(shell);
}

स्थिरांक अक्षर* default_मूळ_shell_at(पूर्णांक rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        प्रतिज्ञा(rfd >= 0 || rfd == AT_FDCWD);

        पूर्णांक r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, रिक्त, रिक्त);
        जर (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        जर (r > 0)
                परत DEFAULT_USER_SHELL;

        परत "/bin/sh";
}

स्थिरांक अक्षर* default_मूळ_shell(स्थिरांक अक्षर *मूळ) {
        _cleanup_close_ पूर्णांक rfd = -EBADF;

        rfd = खोलो(empty_to_मूळ(मूळ), O_CLOEXEC | O_DIRECTORY | O_PATH);
        जर (rfd < 0)
                परत "/bin/sh";

        परत default_मूळ_shell_at(rfd);
}

स्थिर पूर्णांक return_user_creds(
                स्थिरांक अक्षर *username,
                uid_t uid, gid_t gid,
                स्थिरांक अक्षर *home,
                स्थिरांक अक्षर *shell,
                अक्षर **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                अक्षर **ret_home,
                अक्षर **ret_shell) {
        /* Helper function to help with the strdups and atomic setting of return params. */

        _cleanup_free_ अक्षर *s1 = रिक्त, *s2 = रिक्त, *s3 = रिक्त;
        पूर्णांक r;

        जर (ret_username) {
                r = strdup_to(&s1, username);
                जर (r < 0)
                        परत r;
        }

        जर (ret_home) {
                r = strdup_to(&s2, home);
                जर (r < 0)
                        परत r;
        }

        जर (ret_shell) {
                r = strdup_to(&s3, shell);
                जर (r < 0)
                        परत r;
        }

        जर (ret_username)
                *ret_username = TAKE_PTR(s1);
        जर (ret_uid)
                *ret_uid = uid;
        जर (ret_gid)
                *ret_gid = gid;
        जर (ret_home)
                *ret_home = TAKE_PTR(s2);
        जर (ret_shell)
                *ret_shell = TAKE_PTR(s3);
        परत 0;
}

स्थिर पूर्णांक synthesize_user_creds(
                स्थिरांक अक्षर *username,
                UserCredsFlags flags,
                अक्षर **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                अक्षर **ret_home,
                अक्षर **ret_shell) {
        प्रतिज्ञा(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        जर (STR_IN_SET(username, "root", "0"))
                परत return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_मूळ_shell(रिक्त) : रिक्त,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        जर (STR_IN_SET(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                परत return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? रिक्त : "/",
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? रिक्त : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        परत -ENOMEDIUM;
}

पूर्णांक get_user_creds(
                स्थिरांक अक्षर *username,
                UserCredsFlags flags,
                अक्षर **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                अक्षर **ret_home,
                अक्षर **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ संरचना passwd *pw = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(username);
        प्रतिज्ञा((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        जर (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS) ||
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
                जर (r >= 0)
                        परत 0;
                जर (r != -ENOMEDIUM) /* not a username we can synthesize */
                        परत r;
        }

        जर (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                जर (r >= 0)
                        username = pw->pw_name;

                अन्यथा जर (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        जर (ret_username)
                                *ret_username = रिक्त;
                        जर (ret_uid)
                                *ret_uid = u;
                        परत 0;
                }
        } अन्यथा
                r = getpwnam_malloc(username, &pw);

        जर (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                जर (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        परत 0;

                परत r;
        }

        जर (ret_uid && !uid_is_valid(pw->pw_uid))
                परत -EBADMSG;

        जर (ret_gid && !gid_is_valid(pw->pw_gid))
                परत -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        स्थिरांक अक्षर *h =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && empty_or_मूळ(pw->pw_dir)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_dir) || !path_is_absolute(pw->pw_dir)))
                ? रिक्त : pw->pw_dir;

        स्थिरांक अक्षर *s =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_shell) || !path_is_absolute(pw->pw_shell)))
                ? रिक्त : pw->pw_shell;

        परत return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

स्थिर पूर्णांक synthesize_group_creds(स्थिरांक अक्षर *groupname, अक्षर **ret_name, gid_t *ret_gid) {
        प्रतिज्ञा(groupname);

        gid_t id;
        स्थिरांक अक्षर *n;
        पूर्णांक r;

        जर (STR_IN_SET(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } अन्यथा जर (STR_IN_SET(groupname, NOBODY_GROUP_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_GROUP_NAME;
        } अन्यथा
                परत -ENOMEDIUM;

        r = strdup_to_full(ret_name, n);
        जर (r < 0)
                परत r;
        जर (ret_gid)
                *ret_gid = id;
        परत 0;
}

पूर्णांक get_group_creds(स्थिरांक अक्षर *groupname, UserCredsFlags flags, अक्षर **ret_name, gid_t *ret_gid) {
        _cleanup_free_ संरचना group *gr = रिक्त;
        gid_t id;
        पूर्णांक r;

        प्रतिज्ञा(groupname);

        जर (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_group_creds(groupname, ret_name, ret_gid);
                जर (r >= 0)
                        परत 0;
                जर (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        परत r;
        }

        जर (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                जर (r >= 0)
                        groupname = gr->gr_name;
                अन्यथा जर (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING)) {
                        जर (ret_gid)
                                *ret_gid = id;
                        जर (ret_name)
                                *ret_name = रिक्त;
                        परत 0;
                }
        } अन्यथा
                r = getgrnam_malloc(groupname, &gr);

        जर (r < 0) {
                जर (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_group_creds(groupname, ret_name, ret_gid) >= 0)
                        परत 0;
                परत r;
        }

        जर (ret_gid && !gid_is_valid(gr->gr_gid))
                परत -EBADMSG;

        r = strdup_to_full(ret_name, groupname);
        जर (r < 0)
                परत r;
        जर (ret_gid)
                *ret_gid = gr->gr_gid;
        परत 0;
}

अक्षर* uid_to_name(uid_t uid) {
        अक्षर *ret;
        पूर्णांक r;

        /* Shortcut things to avoid NSS lookups */
        जर (uid == 0)
                परत strdup("root");
        जर (uid == UID_NOBODY && synthesize_nobody())
                परत strdup(NOBODY_USER_NAME);

        जर (uid_is_valid(uid)) {
                _cleanup_free_ संरचना passwd *pw = रिक्त;

                r = getpwuid_malloc(uid, &pw);
                जर (r >= 0)
                        परत strdup(pw->pw_name);
        }

        जर (asprintf(&ret, UID_FMT, uid) < 0)
                परत रिक्त;

        परत ret;
}

अक्षर* gid_to_name(gid_t gid) {
        अक्षर *ret;
        पूर्णांक r;

        जर (gid == 0)
                परत strdup("root");
        जर (gid == GID_NOBODY && synthesize_nobody())
                परत strdup(NOBODY_GROUP_NAME);

        जर (gid_is_valid(gid)) {
                _cleanup_free_ संरचना group *gr = रिक्त;

                r = getgrgid_malloc(gid, &gr);
                जर (r >= 0)
                        परत strdup(gr->gr_name);
        }

        जर (asprintf(&ret, GID_FMT, gid) < 0)
                परत रिक्त;

        परत ret;
}

स्थिर bool gid_list_has(स्थिरांक gid_t *list, माप_प्रकार size, gid_t val) {
        प्रतिज्ञा(list || size == 0);

        FOREACH_ARRAY(i, list, size)
                जर (*i == val)
                        परत true;

        परत false;
}

पूर्णांक in_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = रिक्त;
        पूर्णांक ngroups;

        जर (getgid() == gid)
                परत 1;

        जर (getegid() == gid)
                परत 1;

        जर (!gid_is_valid(gid))
                परत -EINVAL;

        ngroups = getgroups_alloc(&gids);
        जर (ngroups < 0)
                परत ngroups;

        परत gid_list_has(gids, ngroups, gid);
}

पूर्णांक in_group(स्थिरांक अक्षर *name) {
        पूर्णांक r;
        gid_t gid;

        r = get_group_creds(name, /* flags= */ 0, /* ret_name= */ रिक्त, &gid);
        जर (r < 0)
                परत r;

        परत in_gid(gid);
}

पूर्णांक merge_gid_lists(स्थिरांक gid_t *list1, माप_प्रकार size1, स्थिरांक gid_t *list2, माप_प्रकार size2, gid_t **ret) {
        माप_प्रकार nresult = 0;

        प्रतिज्ञा(size1 == 0 || list1);
        प्रतिज्ञा(size2 == 0 || list2);
        प्रतिज्ञा(ret);

        जर (size2 > पूर्णांक_उच्च - size1)
                परत -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        जर (!buf)
                परत -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        साठी (माप_प्रकार i = 0; i < size1; i++)
                जर (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        साठी (माप_प्रकार i = 0; i < size2; i++)
                जर (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        परत (पूर्णांक)nresult;
}

पूर्णांक getgroups_alloc(gid_t **ret) {
        पूर्णांक ngroups = 8;

        प्रतिज्ञा(ret);

        साठी (अचिन्हित attempt = 0;;) {
                _cleanup_free_ gid_t *p = रिक्त;

                p = new(gid_t, ngroups);
                जर (!p)
                        परत -ENOMEM;

                ngroups = getgroups(ngroups, p);
                जर (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        परत ngroups;
                }
                जर (ngroups == 0)
                        थांबा;
                जर (त्रुटि_सं != EINVAL)
                        परत -त्रुटि_सं;

                /* Give up eventually */
                जर (attempt++ > 10)
                        परत -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, रिक्त);
                जर (ngroups < 0)
                        परत -त्रुटि_सं;
                जर (ngroups == 0)
                        थांबा;
        }

        *ret = रिक्त;
        परत 0;
}

पूर्णांक get_home_dir(अक्षर **ret) {
        _cleanup_free_ संरचना passwd *p = रिक्त;
        स्थिरांक अक्षर *e;
        uid_t u;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        जर (e && path_is_valid(e) && path_is_absolute(e))
                जा found;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        जर (u == 0) {
                e = "/root";
                जा found;
        }
        जर (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                जा found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        जर (r < 0)
                परत r;

        e = p->pw_dir;
        जर (!path_is_valid(e) || !path_is_absolute(e))
                परत -EINVAL;

 found:
        परत path_simplify_alloc(e, ret);
}

पूर्णांक get_shell(अक्षर **ret) {
        _cleanup_free_ संरचना passwd *p = रिक्त;
        स्थिरांक अक्षर *e;
        uid_t u;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        जर (e && path_is_valid(e) && path_is_absolute(e))
                जा found;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        जर (u == 0) {
                e = default_मूळ_shell(रिक्त);
                जा found;
        }
        जर (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                जा found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        जर (r < 0)
                परत r;

        e = p->pw_shell;
        जर (!path_is_valid(e) || !path_is_absolute(e))
                परत -EINVAL;

 found:
        परत path_simplify_alloc(e, ret);
}

पूर्णांक fully_set_uid_gid(uid_t uid, gid_t gid, स्थिरांक gid_t supplementary_gids[], माप_प्रकार n_supplementary_gids) {
        पूर्णांक r;

        प्रतिज्ञा(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        जर (r < 0)
                परत r;

        जर (gid_is_valid(gid))
                जर (setresgid(gid, gid, gid) < 0)
                        परत -त्रुटि_सं;

        जर (uid_is_valid(uid))
                जर (setresuid(uid, uid, uid) < 0)
                        परत -त्रुटि_सं;

        परत 0;
}

पूर्णांक take_etc_passwd_lock(स्थिरांक अक्षर *मूळ) {
        पूर्णांक r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ अक्षर *path = path_join(मूळ, ETC_PASSWD_LOCK_PATH);
        जर (!path)
                परत log_oom_debug();

        (शून्य) mkdir_parents(path, 0755);

        _cleanup_close_ पूर्णांक fd = खोलो(path, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        जर (fd < 0)
                परत log_debug_errno(त्रुटि_सं, "Cannot open %s: %m", path);

        r = unposix_lock(fd, LOCK_EX);
        जर (r < 0)
                परत log_debug_errno(r, "Locking %s failed: %m", path);

        परत TAKE_FD(fd);
}

bool valid_user_group_name(स्थिरांक अक्षर *u, ValidUserFlags flags) {
        स्थिरांक अक्षर *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        जर (isempty(u)) /* An empty user name is never valid */
                परत false;

        जर (parse_uid(u, रिक्त) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                परत FLAGS_SET(flags, VALID_USER_ALLOW_NUMERIC);

        जर (FLAGS_SET(flags, VALID_USER_RELAX)) {

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

                जर (startswith(u, " ") || endswith(u, " ")) /* At least expect whitespace padding is removed
                                                             * at front and back (accept in the middle, since
                                                             * that's apparently a thing on Windows). Note
                                                             * that this also blocks usernames consisting of
                                                             * whitespace only. */
                        परत false;

                जर (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        परत false;

                जर (string_has_cc(u, रिक्त)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        परत false;

                जर (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        परत false;

                जर (in_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        परत false;

                जर (u[0] == '-' && in_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        परत false;

                जर (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        परत false;

                /* Compare with strict result and warn if result doesn't match */
                जर (FLAGS_SET(flags, VALID_USER_WARN) && !valid_user_group_name(u, 0))
                        log_struct(LOG_NOTICE,
                                   LOG_MESSAGE("Accepting user/group name '%s', which does not match strict user/group name rules.", u),
                                   LOG_ITEM("USER_GROUP_NAME=%s", u),
                                   LOG_MESSAGE_ID(SD_MESSAGE_UNSAFE_USER_NAME_STR));

                /* Note that we make no restrictions on the length in relaxed mode! */
        } अन्यथा {
                दीर्घ sz;
                माप_प्रकार l;

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

                जर (!ascii_isalpha(u[0]) &&
                    u[0] != '_')
                        परत false;

                साठी (i = u+1; *i; i++)
                        जर (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !IN_SET(*i, '_', '-'))
                                परत false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                जर (l > (माप_प्रकार) sz) /* glibc: 256 */
                        परत false;
                जर (l > NAME_MAX) /* must fit in a filename: 255 */
                        परत false;
                जर (l > sizeof_field(संरचना utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        परत false;
        }

        परत true;
}

bool valid_gecos(स्थिरांक अक्षर *d) {

        जर (!d)
                परत false;

        जर (!utf8_is_valid(d))
                परत false;

        जर (string_has_cc(d, रिक्त))
                परत false;

        /* Colons are used as field separators, and hence not OK */
        जर (म_अक्षर(d, ':'))
                परत false;

        परत true;
}

अक्षर* mangle_gecos(स्थिरांक अक्षर *d) {
        अक्षर *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        जर (!mangled)
                परत रिक्त;

        साठी (अक्षर *i = mangled; *i; i++) {
                पूर्णांक len;

                जर ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        सुरूठेवा;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                जर (len < 0) {
                        *i = ' ';
                        सुरूठेवा;
                }

                i += len - 1;
        }

        परत mangled;
}

bool valid_home(स्थिरांक अक्षर *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        जर (isempty(p))
                परत false;

        जर (!utf8_is_valid(p))
                परत false;

        जर (string_has_cc(p, रिक्त))
                परत false;

        जर (!path_is_absolute(p))
                परत false;

        जर (!path_is_normalized(p))
                परत false;

        /* Colons are used as field separators, and hence not OK */
        जर (म_अक्षर(p, ':'))
                परत false;

        परत true;
}

bool valid_shell(स्थिरांक अक्षर *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        जर (!valid_home(p))
                परत false;

        परत !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

पूर्णांक maybe_setgroups(माप_प्रकार size, स्थिरांक gid_t *list) {
        पूर्णांक r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        जर (size == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ खाता *f = ख_खोलो("/proc/self/gid_map", "re");
                जर (!f && त्रुटि_सं != ENOENT)
                        परत -त्रुटि_सं;
                जर (f) {
                        r = safe_fgetc(f, /* ret= */ रिक्त);
                        जर (r < 0)
                                परत r;
                        जर (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                परत 0;
                        }
                }

                _cleanup_free_ अक्षर *setgroups_content = रिक्त;
                r = read_one_ओळ_file("/proc/self/setgroups", &setgroups_content);
                जर (r < 0 && r != -ENOENT)
                        परत r;
                जर (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        परत 0;
                }
        }

        परत RET_NERRNO(setgroups(size, list));
}

bool synthesize_nobody(शून्य) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        स्थिर पूर्णांक cache = -1;

        जर (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        परत cache;
}

पूर्णांक putpwent_sane(स्थिरांक संरचना passwd *pw, खाता *stream) {
        प्रतिज्ञा(pw);
        प्रतिज्ञा(stream);

        त्रुटि_सं = 0;
        जर (putpwent(pw, stream) != 0)
                परत errno_or_else(EIO);

        परत 0;
}

पूर्णांक putspent_sane(स्थिरांक संरचना spwd *sp, खाता *stream) {
        प्रतिज्ञा(sp);
        प्रतिज्ञा(stream);

        त्रुटि_सं = 0;
        जर (putspent(sp, stream) != 0)
                परत errno_or_else(EIO);

        परत 0;
}

पूर्णांक putgrent_sane(स्थिरांक संरचना group *gr, खाता *stream) {
        प्रतिज्ञा(gr);
        प्रतिज्ञा(stream);

        त्रुटि_सं = 0;
        जर (putgrent(gr, stream) != 0)
                परत errno_or_else(EIO);

        परत 0;
}

#जर ENABLE_GSHADOW
पूर्णांक putsgent_sane(स्थिरांक संरचना sgrp *sg, खाता *stream) {
        प्रतिज्ञा(sg);
        प्रतिज्ञा(stream);

        त्रुटि_सं = 0;
        जर (putsgent(sg, stream) != 0)
                परत errno_or_else(EIO);

        परत 0;
}
#endif

पूर्णांक fgetpwent_sane(खाता *stream, संरचना passwd **pw) {
        प्रतिज्ञा(stream);
        प्रतिज्ञा(pw);

        त्रुटि_सं = 0;
        संरचना passwd *p = fgetpwent(stream);
        जर (!p && !IN_SET(त्रुटि_सं, 0, ENOENT))
                परत -त्रुटि_सं;

        *pw = p;
        परत !!p;
}

पूर्णांक fgetspent_sane(खाता *stream, संरचना spwd **sp) {
        प्रतिज्ञा(stream);
        प्रतिज्ञा(sp);

        त्रुटि_सं = 0;
        संरचना spwd *s = fgetspent(stream);
        जर (!s && !IN_SET(त्रुटि_सं, 0, ENOENT))
                परत -त्रुटि_सं;

        *sp = s;
        परत !!s;
}

पूर्णांक fgetgrent_sane(खाता *stream, संरचना group **gr) {
        प्रतिज्ञा(stream);
        प्रतिज्ञा(gr);

        त्रुटि_सं = 0;
        संरचना group *g = fgetgrent(stream);
        जर (!g && !IN_SET(त्रुटि_सं, 0, ENOENT))
                परत -त्रुटि_सं;

        *gr = g;
        परत !!g;
}

#जर ENABLE_GSHADOW
पूर्णांक fgetsgent_sane(खाता *stream, संरचना sgrp **sg) {
        प्रतिज्ञा(stream);
        प्रतिज्ञा(sg);

        त्रुटि_सं = 0;
        संरचना sgrp *s = fgetsgent(stream);
        जर (!s && !IN_SET(त्रुटि_सं, 0, ENOENT))
                परत -त्रुटि_सं;

        *sg = s;
        परत !!s;
}
#endif

पूर्णांक is_this_me(स्थिरांक अक्षर *username) {
        uid_t uid;
        पूर्णांक r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, रिक्त, &uid, रिक्त, रिक्त, रिक्त);
        जर (r < 0)
                परत r;

        परत uid == getuid();
}

स्थिरांक अक्षर* get_home_मूळ(शून्य) {
        /* For debug purposes allow overriding where we look for home dirs */
        स्थिरांक अक्षर *e = secure_getenv("SYSTEMD_HOME_ROOT");
        जर (e && path_is_absolute(e) && path_is_normalized(e))
                परत e;

        परत "/home";
}

स्थिर पूर्णांक copy_struct_passwd(स्थिरांक संरचना passwd *pw, संरचना passwd **ret) {
        प्रतिज्ञा(pw);
        प्रतिज्ञा(ret);

        माप_प्रकार need_bytes = आकार(संरचना passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        अक्षर *buf = दो_स्मृति(need_bytes);
        जर (!buf)
                परत -ENOMEM;

        संरचना passwd *newpw = (संरचना passwd *) buf;

        /* The layout in our buffer:
         * struct passwd, and then individual strings. */
        अक्षर *p = buf + आकार(संरचना passwd);

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
        परत 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
पूर्णांक lookup_pwent_in_files(
                अक्षर * स्थिरांक *files,
                स्थिरांक अक्षर *name,
                uid_t uid,
                संरचना passwd **ret) {

        पूर्णांक r;

        प्रतिज्ञा(files);
        प्रतिज्ञा(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ खाता *f = रिक्त;
                संरचना passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                जर (r == -ENOENT)
                        सुरूठेवा;
                जर (r < 0)
                        परत r;

                पर्यंत ((r = fgetpwent_sane(f, &pw)) > 0) {
                        जर (name && !streq_ptr(pw->pw_name, name))
                                सुरूठेवा;
                        जर (uid_is_valid(uid) && pw->pw_uid != uid)
                                सुरूठेवा;
                        जर (ret)
                                परत copy_struct_passwd(pw, ret);
                        परत 0;
                }
                जर (r < 0)
                        परत r;
        }

        परत -ESRCH;
}

#जर !BUILD_STATIC

स्थिर माप_प्रकार getpw_buffer_size(शून्य) {
        दीर्घ bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        परत bufsize <= 0 ? 4096U : (माप_प्रकार) bufsize;
}

स्थिर bool errno_is_user_doesnt_exist(पूर्णांक error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        परत IN_SET(असल(error), ENOENT, ESRCH, EBADF, EPERM);
}

#endif

पूर्णांक getpwnam_malloc(स्थिरांक अक्षर *name, संरचना passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        जर (isempty(name))
                परत -EINVAL;

#जर BUILD_STATIC
        परत lookup_pwent_in_files(PASSWD_FILES, name, UID_INVALID, ret);
#अन्यथा
        माप_प्रकार bufsize = getpw_buffer_size();
        पूर्णांक r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        साठी (;;) {
                _cleanup_free_ शून्य *buf = रिक्त;

                /* Silence static analyzers */
                प्रतिज्ञा(bufsize <= SIZE_MAX - ALIGN(आकार(संरचना passwd)));
                buf = malloc0(ALIGN(आकार(संरचना passwd)) + bufsize);
                जर (!buf)
                        परत -ENOMEM;

                संरचना passwd *pw = रिक्त;
                r = getpwnam_r(name, buf, (अक्षर*) buf + ALIGN(आकार(संरचना passwd)), bufsize, &pw);
                जर (r == 0) {
                        जर (pw) {
                                जर (ret)
                                        *ret = TAKE_PTR(buf);
                                परत 0;
                        }

                        परत -ESRCH;
                }

                प्रतिज्ञा(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                जर (errno_is_user_doesnt_exist(r))
                        परत -ESRCH;
                जर (r != दुस्फल)
                        परत -r;

                जर (bufsize > SIZE_MAX/2 - ALIGN(आकार(संरचना passwd)))
                        परत -ENOMEM;
                bufsize *= 2;
        }
#endif
}

पूर्णांक getpwuid_malloc(uid_t uid, संरचना passwd **ret) {
        जर (!uid_is_valid(uid))
                परत -EINVAL;

#जर BUILD_STATIC
        परत lookup_pwent_in_files(PASSWD_FILES, /* name= */ रिक्त, uid, ret);
#अन्यथा
        माप_प्रकार bufsize = getpw_buffer_size();
        पूर्णांक r;

        साठी (;;) {
                _cleanup_free_ शून्य *buf = रिक्त;

                /* Silence static analyzers */
                प्रतिज्ञा(bufsize <= SIZE_MAX - ALIGN(आकार(संरचना passwd)));
                buf = malloc0(ALIGN(आकार(संरचना passwd)) + bufsize);
                जर (!buf)
                        परत -ENOMEM;

                संरचना passwd *pw = रिक्त;
                r = getpwuid_r(uid, buf, (अक्षर*) buf + ALIGN(आकार(संरचना passwd)), bufsize, &pw);
                जर (r == 0) {
                        जर (pw) {
                                जर (ret)
                                        *ret = TAKE_PTR(buf);
                                परत 0;
                        }

                        परत -ESRCH;
                }

                प्रतिज्ञा(r > 0);

                जर (errno_is_user_doesnt_exist(r))
                        परत -ESRCH;
                जर (r != दुस्फल)
                        परत -r;

                जर (bufsize > SIZE_MAX/2 - ALIGN(आकार(संरचना passwd)))
                        परत -ENOMEM;
                bufsize *= 2;
        }
#endif
}

स्थिर पूर्णांक copy_struct_group(स्थिरांक संरचना group *gr, संरचना group **ret) {
        प्रतिज्ञा(gr);
        प्रतिज्ञा(ret);

        माप_प्रकार need_bytes = आकार(संरचना group)
                + strlen_ptr(gr->gr_name) + 1
                + strlen_ptr(gr->gr_passwd) + 1,
                n_mem = 0;
        STRV_FOREACH(s, gr->gr_mem) {
                need_bytes += आकार(अक्षर*) + म_माप(*s) + 1;
                n_mem++;
        }
        need_bytes += आकार(अक्षर*);  /* NULL terminator for gr_mem */

        अक्षर *buf = दो_स्मृति(need_bytes);
        जर (!buf)
                परत -ENOMEM;

        संरचना group *newgr = (संरचना group *) buf;

        /* The layout in our buffer:
         * struct group, ->gr_mem pointers terminated by NULL, ->gr_name, ->gr_passwd, ->gr_mem items */
        /* The ->gr_mem array is first, because it needs alignment. */
        assert_cc(आकार(संरचना group) % alignof(अक्षर*) == 0);

        अक्षर *p = buf + आकार(संरचना group) + (n_mem + 1) * आकार(अक्षर*);

        newgr->gr_name = p;
        p = stpcpy(p, strempty(gr->gr_name)) + 1;

        newgr->gr_passwd = p;
        p = stpcpy(p, strempty(gr->gr_passwd)) + 1;

        newgr->gr_gid = gr->gr_gid;

        newgr->gr_mem = (अक्षर**) (buf + आकार(संरचना group));
        साठी (माप_प्रकार i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = रिक्त;

        *ret = newgr;
        परत 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
पूर्णांक lookup_grent_in_files(
                अक्षर * स्थिरांक *files,
                स्थिरांक अक्षर *name,
                gid_t gid,
                संरचना group **ret) {

        पूर्णांक r;

        प्रतिज्ञा(files);
        प्रतिज्ञा(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ खाता *f = रिक्त;
                संरचना group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                जर (r == -ENOENT)
                        सुरूठेवा;
                जर (r < 0)
                        परत r;

                पर्यंत ((r = fgetgrent_sane(f, &gr)) > 0) {
                        जर (name && !streq_ptr(gr->gr_name, name))
                                सुरूठेवा;
                        जर (gid_is_valid(gid) && gr->gr_gid != gid)
                                सुरूठेवा;
                        जर (ret)
                                परत copy_struct_group(gr, ret);
                        परत 0;
                }
                जर (r < 0)
                        परत r;
        }

        परत -ESRCH;
}

पूर्णांक sysconf_ngroups_max(शून्य) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        त्रुटि_सं = 0;
        दीर्घ ngroups_max = sysconf(_SC_NGROUPS_MAX);
        जर (ngroups_max <= 0)
                परत errno_or_else(EOPNOTSUPP);
        जर (ngroups_max > पूर्णांक_उच्च)
                परत -दुस्फल;
        परत ngroups_max;
}

ssize_t lookup_groups_in_files(
                अक्षर * स्थिरांक *files,
                स्थिरांक अक्षर *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = रिक्त;
        ssize_t n_arr = 0;
        पूर्णांक r;

        प्रतिज्ञा(files);
        प्रतिज्ञा(name);
        प्रतिज्ञा(gid_is_valid(gid));

        पूर्णांक ngroups_max = sysconf_ngroups_max();
        जर (ngroups_max < 0)
                परत ngroups_max;

        जर (!GREEDY_REALLOC(arr, n_arr + 1))
                परत -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ खाता *f = रिक्त;
                संरचना group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                जर (r == -ENOENT)
                        सुरूठेवा;
                जर (r < 0)
                        परत r;

                पर्यंत ((r = fgetgrent_sane(f, &gr)) > 0) {
                        जर (gid == gr->gr_gid)
                                सुरूठेवा;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        जर (gid_list_has(arr, n_arr, gr->gr_gid))
                                सुरूठेवा;

                        जर (!strv_contains(gr->gr_mem, name))
                                सुरूठेवा;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        जर (n_arr >= ngroups_max)
                                परत -ENOBUFS;

                        जर (!GREEDY_REALLOC(arr, n_arr + 1))
                                परत -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                जर (r < 0)
                        परत r;
        }

        जर (ret)
                *ret = TAKE_PTR(arr);
        परत n_arr;
}

पूर्णांक getgrouplist_malloc(स्थिरांक अक्षर *user, gid_t gid, gid_t **ret) {
#जर BUILD_STATIC
        परत lookup_groups_in_files(GROUP_FILES, user, gid, ret);
#अन्यथा
        पूर्णांक ngroups_max = sysconf_ngroups_max();
        जर (ngroups_max < 0)
                परत ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        जर (!gids)
                परत -ENOMEM;

        पूर्णांक k = ngroups_max;
        जर (getgrouplist(user, gid, gids, &k) < 0)
                परत -EIO;  /* getgrouplist does not set errno */

        जर (ret)
                *ret = TAKE_PTR(gids);
        परत k;
#endif
}

पूर्णांक initgroups_wrapper(स्थिरांक अक्षर *user, gid_t gid) {
#जर BUILD_STATIC
        _cleanup_free_ gid_t *groups = रिक्त;
        पूर्णांक r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        जर (n_groups <= 0)
                परत n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        साठी (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                जर (r != -EINVAL)
                        थांबा;
        }

        परत r;
#अन्यथा
        परत RET_NERRNO(initgroups(user, gid));
#endif
}

#जर !BUILD_STATIC

स्थिर माप_प्रकार getgr_buffer_size(शून्य) {
        दीर्घ bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        परत bufsize <= 0 ? 4096U : (माप_प्रकार) bufsize;
}

#endif

पूर्णांक getgrnam_malloc(स्थिरांक अक्षर *name, संरचना group **ret) {
        जर (isempty(name))
                परत -EINVAL;

#जर BUILD_STATIC
        परत lookup_grent_in_files(GROUP_FILES, name, GID_INVALID, ret);
#अन्यथा
        माप_प्रकार bufsize = getgr_buffer_size();
        पूर्णांक r;

        साठी (;;) {
                _cleanup_free_ शून्य *buf = रिक्त;

                /* Silence static analyzers */
                प्रतिज्ञा(bufsize <= SIZE_MAX - ALIGN(आकार(संरचना group)));
                buf = malloc0(ALIGN(आकार(संरचना group)) + bufsize);
                जर (!buf)
                        परत -ENOMEM;

                संरचना group *gr = रिक्त;
                r = getgrnam_r(name, buf, (अक्षर*) buf + ALIGN(आकार(संरचना group)), bufsize, &gr);
                जर (r == 0) {
                        जर (gr) {
                                जर (ret)
                                        *ret = TAKE_PTR(buf);
                                परत 0;
                        }

                        परत -ESRCH;
                }

                प्रतिज्ञा(r > 0);

                जर (errno_is_user_doesnt_exist(r))
                        परत -ESRCH;
                जर (r != दुस्फल)
                        परत -r;

                जर (bufsize > SIZE_MAX/2 - ALIGN(आकार(संरचना group)))
                        परत -ENOMEM;
                bufsize *= 2;
        }
#endif
}

पूर्णांक getgrgid_malloc(gid_t gid, संरचना group **ret) {
        जर (!gid_is_valid(gid))
                परत -EINVAL;

#जर BUILD_STATIC
        परत lookup_grent_in_files(GROUP_FILES, /* name= */ रिक्त, gid, ret);
#अन्यथा
        माप_प्रकार bufsize = getgr_buffer_size();
        पूर्णांक r;

        साठी (;;) {
                _cleanup_free_ शून्य *buf = रिक्त;

                /* Silence static analyzers */
                प्रतिज्ञा(bufsize <= SIZE_MAX - ALIGN(आकार(संरचना group)));
                buf = malloc0(ALIGN(आकार(संरचना group)) + bufsize);
                जर (!buf)
                        परत -ENOMEM;

                संरचना group *gr = रिक्त;
                r = getgrgid_r(gid, buf, (अक्षर*) buf + ALIGN(आकार(संरचना group)), bufsize, &gr);
                जर (r == 0) {
                        जर (gr) {
                                जर (ret)
                                        *ret = TAKE_PTR(buf);
                                परत 0;
                        }

                        परत -ESRCH;
                }

                प्रतिज्ञा(r > 0);

                जर (errno_is_user_doesnt_exist(r))
                        परत -ESRCH;
                जर (r != दुस्फल)
                        परत -r;

                जर (bufsize > SIZE_MAX/2 - ALIGN(आकार(संरचना group)))
                        परत -ENOMEM;
                bufsize *= 2;
        }
#endif
}
