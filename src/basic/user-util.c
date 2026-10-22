/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <fcntl.h>
#समावेश <stdio.h>
#समावेश <stdlib.h>
#समावेश <sys/file.h>
#समावेश <sys/stat.h>
#समावेश <unistd.h>
#समावेश <utmpx.h>

#समावेश "sd-messages.h"

#समावेश "alloc-util.h"
#समावेश "chase.h"
#समावेश "errno-util.h"
#समावेश "extract-word.h"
#समावेश "fd-util.h"
#समावेश "fileio.h"
#समावेश "format-util.h"
#समावेश "lock-util.h"
#समावेश "log.h"
#समावेश "mkdir.h"
#समावेश "parse-util.h"
#समावेश "path-util.h"
#समावेश "string-util.h"
#समावेश "strv.h"
#समावेश "terminal-util.h"
#समावेश "user-util.h"
#समावेश "utf8.h"

#घोषणा DEFINE_STRERROR_ACCOUNT(type)                                   \
        स्थिर अक्षर* strerror_##type(                                    \
                        पूर्णांक errnum,                                     \
                        अक्षर *buf,                                      \
                        माप_प्रकार buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                चयन (errnum) {                                       \
                हाल ESRCH:                                             \
                        वापस "Unknown " STRINGIFY(type);              \
                हाल ENOEXEC:                                           \
                        वापस "Not a system " STRINGIFY(type);         \
                शेष:                                                \
                        वापस strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        अगर (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                वापस false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        अगर (uid == (uid_t) UINT32_C(0xFFFF))
                वापस false;

        वापस true;
}

पूर्णांक parse_uid(स्थिर अक्षर *s, uid_t *ret) {
        uint32_t uid = 0;
        पूर्णांक r;

        निश्चित(s);

        assert_cc(माप(uid_t) == माप(uint32_t));

        /* We are very strict when parsing UIDs, and prohibit +/- as prefix, leading zero as prefix, and
         * whitespace. We do this, since this call is often used in a context where we parse things as UID
         * first, and if that doesn't work we fall back to NSS. Thus we really want to make sure that UIDs
         * are parsed as UIDs only if they really really look like UIDs. */
        r = safe_atou32_full(s, 10
                             | SAFE_ATO_REFUSE_PLUS_MINUS
                             | SAFE_ATO_REFUSE_LEADING_ZERO
                             | SAFE_ATO_REFUSE_LEADING_WHITESPACE, &uid);
        अगर (r < 0)
                वापस r;

        अगर (!uid_is_valid(uid))
                वापस -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        अगर (ret)
                *ret = uid;

        वापस 0;
}

पूर्णांक parse_uid_range(स्थिर अक्षर *s, uid_t *ret_lower, uid_t *ret_upper) {
        _cleanup_free_ अक्षर *word = शून्य;
        uid_t l, u;
        पूर्णांक r;

        निश्चित(s);
        निश्चित(ret_lower);
        निश्चित(ret_upper);

        r = extract_first_word(&s, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        अगर (r < 0)
                वापस r;
        अगर (r == 0)
                वापस -EINVAL;

        r = parse_uid(word, &l);
        अगर (r < 0)
                वापस r;

        /* Check for the upper bound and extract it if needed */
        अगर (!s)
                /* Single number with no dash. */
                u = l;
        अन्यथा अगर (!*s)
                /* Trailing dash is an error. */
                वापस -EINVAL;
        अन्यथा {
                r = parse_uid(s, &u);
                अगर (r < 0)
                        वापस r;

                अगर (l > u)
                        वापस -EINVAL;
        }

        *ret_lower = l;
        *ret_upper = u;
        वापस 0;
}

अक्षर* getlogname_malloc(व्योम) {
        uid_t uid;
        काष्ठा stat st;

        अगर (isatty_safe(STDIN_FILENO) && ख_स्थिति(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        अन्यथा
                uid = getuid();

        वापस uid_to_name(uid);
}

अक्षर* getusername_malloc(व्योम) {
        स्थिर अक्षर *e;

        e = secure_getenv("USER");
        अगर (e)
                वापस strdup(e);

        वापस uid_to_name(getuid());
}

bool is_nologin_shell(स्थिर अक्षर *shell) {
        वापस PATH_IN_SET(shell,
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

bool shell_is_placeholder(स्थिर अक्षर *shell) {
        वापस isempty(shell) || is_nologin_shell(shell);
}

स्थिर अक्षर* default_मूल_shell_at(पूर्णांक rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        निश्चित(rfd >= 0 || rfd == AT_FDCWD);

        पूर्णांक r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, शून्य, शून्य);
        अगर (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        अगर (r > 0)
                वापस DEFAULT_USER_SHELL;

        वापस "/bin/sh";
}

स्थिर अक्षर* default_मूल_shell(स्थिर अक्षर *मूल) {
        _cleanup_close_ पूर्णांक rfd = -EBADF;

        rfd = खोलो(empty_to_मूल(मूल), O_CLOEXEC | O_DIRECTORY | O_PATH);
        अगर (rfd < 0)
                वापस "/bin/sh";

        वापस default_मूल_shell_at(rfd);
}

अटल पूर्णांक return_user_creds(
                स्थिर अक्षर *username,
                uid_t uid, gid_t gid,
                स्थिर अक्षर *home,
                स्थिर अक्षर *shell,
                अक्षर **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                अक्षर **ret_home,
                अक्षर **ret_shell) {
        /* Helper function to help with the strdups and atomic setting of return params. */

        _cleanup_free_ अक्षर *s1 = शून्य, *s2 = शून्य, *s3 = शून्य;
        पूर्णांक r;

        अगर (ret_username) {
                r = strdup_to(&s1, username);
                अगर (r < 0)
                        वापस r;
        }

        अगर (ret_home) {
                r = strdup_to(&s2, home);
                अगर (r < 0)
                        वापस r;
        }

        अगर (ret_shell) {
                r = strdup_to(&s3, shell);
                अगर (r < 0)
                        वापस r;
        }

        अगर (ret_username)
                *ret_username = TAKE_PTR(s1);
        अगर (ret_uid)
                *ret_uid = uid;
        अगर (ret_gid)
                *ret_gid = gid;
        अगर (ret_home)
                *ret_home = TAKE_PTR(s2);
        अगर (ret_shell)
                *ret_shell = TAKE_PTR(s3);
        वापस 0;
}

अटल पूर्णांक synthesize_user_creds(
                स्थिर अक्षर *username,
                UserCredsFlags flags,
                अक्षर **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                अक्षर **ret_home,
                अक्षर **ret_shell) {
        निश्चित(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        अगर (STR_IN_SET(username, "root", "0"))
                वापस return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_मूल_shell(शून्य) : शून्य,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        अगर (STR_IN_SET(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                वापस return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? शून्य : "/",
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? शून्य : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        वापस -ENOMEDIUM;
}

पूर्णांक get_user_creds(
                स्थिर अक्षर *username,
                UserCredsFlags flags,
                अक्षर **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                अक्षर **ret_home,
                अक्षर **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ काष्ठा passwd *pw = शून्य;
        पूर्णांक r;

        निश्चित(username);
        निश्चित((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        अगर (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS) ||
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
                अगर (r >= 0)
                        वापस 0;
                अगर (r != -ENOMEDIUM) /* not a username we can synthesize */
                        वापस r;
        }

        अगर (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                अगर (r >= 0)
                        username = pw->pw_name;

                अन्यथा अगर (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        अगर (ret_username)
                                *ret_username = शून्य;
                        अगर (ret_uid)
                                *ret_uid = u;
                        वापस 0;
                }
        } अन्यथा
                r = getpwnam_malloc(username, &pw);

        अगर (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                अगर (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        वापस 0;

                वापस r;
        }

        अगर (ret_uid && !uid_is_valid(pw->pw_uid))
                वापस -EBADMSG;

        अगर (ret_gid && !gid_is_valid(pw->pw_gid))
                वापस -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        स्थिर अक्षर *h =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && empty_or_मूल(pw->pw_dir)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_dir) || !path_is_absolute(pw->pw_dir)))
                ? शून्य : pw->pw_dir;

        स्थिर अक्षर *s =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_shell) || !path_is_absolute(pw->pw_shell)))
                ? शून्य : pw->pw_shell;

        वापस return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

अटल पूर्णांक synthesize_group_creds(स्थिर अक्षर *groupname, अक्षर **ret_name, gid_t *ret_gid) {
        निश्चित(groupname);

        gid_t id;
        स्थिर अक्षर *n;
        पूर्णांक r;

        अगर (STR_IN_SET(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } अन्यथा अगर (STR_IN_SET(groupname, NOBODY_GROUP_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_GROUP_NAME;
        } अन्यथा
                वापस -ENOMEDIUM;

        r = strdup_to_full(ret_name, n);
        अगर (r < 0)
                वापस r;
        अगर (ret_gid)
                *ret_gid = id;
        वापस 0;
}

पूर्णांक get_group_creds(स्थिर अक्षर *groupname, UserCredsFlags flags, अक्षर **ret_name, gid_t *ret_gid) {
        _cleanup_free_ काष्ठा group *gr = शून्य;
        gid_t id;
        पूर्णांक r;

        निश्चित(groupname);

        अगर (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_group_creds(groupname, ret_name, ret_gid);
                अगर (r >= 0)
                        वापस 0;
                अगर (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        वापस r;
        }

        अगर (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                अगर (r >= 0)
                        groupname = gr->gr_name;
                अन्यथा अगर (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING)) {
                        अगर (ret_gid)
                                *ret_gid = id;
                        अगर (ret_name)
                                *ret_name = शून्य;
                        वापस 0;
                }
        } अन्यथा
                r = getgrnam_malloc(groupname, &gr);

        अगर (r < 0) {
                अगर (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_group_creds(groupname, ret_name, ret_gid) >= 0)
                        वापस 0;
                वापस r;
        }

        अगर (ret_gid && !gid_is_valid(gr->gr_gid))
                वापस -EBADMSG;

        r = strdup_to_full(ret_name, groupname);
        अगर (r < 0)
                वापस r;
        अगर (ret_gid)
                *ret_gid = gr->gr_gid;
        वापस 0;
}

अक्षर* uid_to_name(uid_t uid) {
        अक्षर *ret;
        पूर्णांक r;

        /* Shortcut things to avoid NSS lookups */
        अगर (uid == 0)
                वापस strdup("root");
        अगर (uid == UID_NOBODY && synthesize_nobody())
                वापस strdup(NOBODY_USER_NAME);

        अगर (uid_is_valid(uid)) {
                _cleanup_free_ काष्ठा passwd *pw = शून्य;

                r = getpwuid_malloc(uid, &pw);
                अगर (r >= 0)
                        वापस strdup(pw->pw_name);
        }

        अगर (asprintf(&ret, UID_FMT, uid) < 0)
                वापस शून्य;

        वापस ret;
}

अक्षर* gid_to_name(gid_t gid) {
        अक्षर *ret;
        पूर्णांक r;

        अगर (gid == 0)
                वापस strdup("root");
        अगर (gid == GID_NOBODY && synthesize_nobody())
                वापस strdup(NOBODY_GROUP_NAME);

        अगर (gid_is_valid(gid)) {
                _cleanup_free_ काष्ठा group *gr = शून्य;

                r = getgrgid_malloc(gid, &gr);
                अगर (r >= 0)
                        वापस strdup(gr->gr_name);
        }

        अगर (asprintf(&ret, GID_FMT, gid) < 0)
                वापस शून्य;

        वापस ret;
}

अटल bool gid_list_has(स्थिर gid_t *list, माप_प्रकार आकार, gid_t val) {
        निश्चित(list || आकार == 0);

        FOREACH_ARRAY(i, list, आकार)
                अगर (*i == val)
                        वापस true;

        वापस false;
}

पूर्णांक in_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = शून्य;
        पूर्णांक ngroups;

        अगर (getgid() == gid)
                वापस 1;

        अगर (getegid() == gid)
                वापस 1;

        अगर (!gid_is_valid(gid))
                वापस -EINVAL;

        ngroups = getgroups_alloc(&gids);
        अगर (ngroups < 0)
                वापस ngroups;

        वापस gid_list_has(gids, ngroups, gid);
}

पूर्णांक in_group(स्थिर अक्षर *name) {
        पूर्णांक r;
        gid_t gid;

        r = get_group_creds(name, /* flags= */ 0, /* ret_name= */ शून्य, &gid);
        अगर (r < 0)
                वापस r;

        वापस in_gid(gid);
}

पूर्णांक merge_gid_lists(स्थिर gid_t *list1, माप_प्रकार size1, स्थिर gid_t *list2, माप_प्रकार size2, gid_t **ret) {
        माप_प्रकार nresult = 0;

        निश्चित(size1 == 0 || list1);
        निश्चित(size2 == 0 || list2);
        निश्चित(ret);

        अगर (size2 > पूर्णांक_उच्च - size1)
                वापस -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        अगर (!buf)
                वापस -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        क्रम (माप_प्रकार i = 0; i < size1; i++)
                अगर (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        क्रम (माप_प्रकार i = 0; i < size2; i++)
                अगर (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        वापस (पूर्णांक)nresult;
}

पूर्णांक getgroups_alloc(gid_t **ret) {
        पूर्णांक ngroups = 8;

        निश्चित(ret);

        क्रम (अचिन्हित attempt = 0;;) {
                _cleanup_free_ gid_t *p = शून्य;

                p = new(gid_t, ngroups);
                अगर (!p)
                        वापस -ENOMEM;

                ngroups = getgroups(ngroups, p);
                अगर (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        वापस ngroups;
                }
                अगर (ngroups == 0)
                        अवरोध;
                अगर (त्रुटि_सं != EINVAL)
                        वापस -त्रुटि_सं;

                /* Give up eventually */
                अगर (attempt++ > 10)
                        वापस -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, शून्य);
                अगर (ngroups < 0)
                        वापस -त्रुटि_सं;
                अगर (ngroups == 0)
                        अवरोध;
        }

        *ret = शून्य;
        वापस 0;
}

पूर्णांक get_home_dir(अक्षर **ret) {
        _cleanup_free_ काष्ठा passwd *p = शून्य;
        स्थिर अक्षर *e;
        uid_t u;
        पूर्णांक r;

        निश्चित(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        अगर (e && path_is_valid(e) && path_is_absolute(e))
                जाओ found;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        अगर (u == 0) {
                e = "/root";
                जाओ found;
        }
        अगर (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                जाओ found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        अगर (r < 0)
                वापस r;

        e = p->pw_dir;
        अगर (!path_is_valid(e) || !path_is_absolute(e))
                वापस -EINVAL;

 found:
        वापस path_simplify_alloc(e, ret);
}

पूर्णांक get_shell(अक्षर **ret) {
        _cleanup_free_ काष्ठा passwd *p = शून्य;
        स्थिर अक्षर *e;
        uid_t u;
        पूर्णांक r;

        निश्चित(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        अगर (e && path_is_valid(e) && path_is_absolute(e))
                जाओ found;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        अगर (u == 0) {
                e = default_मूल_shell(शून्य);
                जाओ found;
        }
        अगर (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                जाओ found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        अगर (r < 0)
                वापस r;

        e = p->pw_shell;
        अगर (!path_is_valid(e) || !path_is_absolute(e))
                वापस -EINVAL;

 found:
        वापस path_simplify_alloc(e, ret);
}

पूर्णांक fully_set_uid_gid(uid_t uid, gid_t gid, स्थिर gid_t supplementary_gids[], माप_प्रकार n_supplementary_gids) {
        पूर्णांक r;

        निश्चित(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        अगर (r < 0)
                वापस r;

        अगर (gid_is_valid(gid))
                अगर (setresgid(gid, gid, gid) < 0)
                        वापस -त्रुटि_सं;

        अगर (uid_is_valid(uid))
                अगर (setresuid(uid, uid, uid) < 0)
                        वापस -त्रुटि_सं;

        वापस 0;
}

पूर्णांक take_etc_passwd_lock(स्थिर अक्षर *मूल) {
        पूर्णांक r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ अक्षर *path = path_join(मूल, ETC_PASSWD_LOCK_PATH);
        अगर (!path)
                वापस log_oom_debug();

        (व्योम) mkdir_parents(path, 0755);

        _cleanup_close_ पूर्णांक fd = खोलो(path, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        अगर (fd < 0)
                वापस log_debug_errno(त्रुटि_सं, "Cannot open %s: %m", path);

        r = unposix_lock(fd, LOCK_EX);
        अगर (r < 0)
                वापस log_debug_errno(r, "Locking %s failed: %m", path);

        वापस TAKE_FD(fd);
}

bool valid_user_group_name(स्थिर अक्षर *u, ValidUserFlags flags) {
        स्थिर अक्षर *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        अगर (isempty(u)) /* An empty user name is never valid */
                वापस false;

        अगर (parse_uid(u, शून्य) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                वापस FLAGS_SET(flags, VALID_USER_ALLOW_NUMERIC);

        अगर (FLAGS_SET(flags, VALID_USER_RELAX)) {

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

                अगर (startswith(u, " ") || endswith(u, " ")) /* At least expect whitespace padding is removed
                                                             * at front and back (accept in the middle, since
                                                             * that's apparently a thing on Windows). Note
                                                             * that this also blocks usernames consisting of
                                                             * whitespace only. */
                        वापस false;

                अगर (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        वापस false;

                अगर (string_has_cc(u, शून्य)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        वापस false;

                अगर (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        वापस false;

                अगर (in_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        वापस false;

                अगर (u[0] == '-' && in_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        वापस false;

                अगर (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        वापस false;

                /* Compare with strict result and warn if result doesn't match */
                अगर (FLAGS_SET(flags, VALID_USER_WARN) && !valid_user_group_name(u, 0))
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

                अगर (!ascii_isalpha(u[0]) &&
                    u[0] != '_')
                        वापस false;

                क्रम (i = u+1; *i; i++)
                        अगर (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !IN_SET(*i, '_', '-'))
                                वापस false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                अगर (l > (माप_प्रकार) sz) /* glibc: 256 */
                        वापस false;
                अगर (l > NAME_MAX) /* must fit in a filename: 255 */
                        वापस false;
                अगर (l > sizeof_field(काष्ठा utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        वापस false;
        }

        वापस true;
}

bool valid_gecos(स्थिर अक्षर *d) {

        अगर (!d)
                वापस false;

        अगर (!utf8_is_valid(d))
                वापस false;

        अगर (string_has_cc(d, शून्य))
                वापस false;

        /* Colons are used as field separators, and hence not OK */
        अगर (म_अक्षर(d, ':'))
                वापस false;

        वापस true;
}

अक्षर* mangle_gecos(स्थिर अक्षर *d) {
        अक्षर *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        अगर (!mangled)
                वापस शून्य;

        क्रम (अक्षर *i = mangled; *i; i++) {
                पूर्णांक len;

                अगर ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        जारी;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                अगर (len < 0) {
                        *i = ' ';
                        जारी;
                }

                i += len - 1;
        }

        वापस mangled;
}

bool valid_home(स्थिर अक्षर *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        अगर (isempty(p))
                वापस false;

        अगर (!utf8_is_valid(p))
                वापस false;

        अगर (string_has_cc(p, शून्य))
                वापस false;

        अगर (!path_is_absolute(p))
                वापस false;

        अगर (!path_is_normalized(p))
                वापस false;

        /* Colons are used as field separators, and hence not OK */
        अगर (म_अक्षर(p, ':'))
                वापस false;

        वापस true;
}

bool valid_shell(स्थिर अक्षर *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        अगर (!valid_home(p))
                वापस false;

        वापस !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

पूर्णांक maybe_setgroups(माप_प्रकार आकार, स्थिर gid_t *list) {
        पूर्णांक r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        अगर (आकार == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ खाता *f = ख_खोलो("/proc/self/gid_map", "re");
                अगर (!f && त्रुटि_सं != ENOENT)
                        वापस -त्रुटि_सं;
                अगर (f) {
                        r = safe_fgetc(f, /* ret= */ शून्य);
                        अगर (r < 0)
                                वापस r;
                        अगर (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                वापस 0;
                        }
                }

                _cleanup_free_ अक्षर *setgroups_content = शून्य;
                r = read_one_पंक्ति_file("/proc/self/setgroups", &setgroups_content);
                अगर (r < 0 && r != -ENOENT)
                        वापस r;
                अगर (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        वापस 0;
                }
        }

        वापस RET_NERRNO(setgroups(आकार, list));
}

bool synthesize_nobody(व्योम) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        अटल पूर्णांक cache = -1;

        अगर (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        वापस cache;
}

पूर्णांक putpwent_sane(स्थिर काष्ठा passwd *pw, खाता *stream) {
        निश्चित(pw);
        निश्चित(stream);

        त्रुटि_सं = 0;
        अगर (putpwent(pw, stream) != 0)
                वापस errno_or_else(EIO);

        वापस 0;
}

पूर्णांक putspent_sane(स्थिर काष्ठा spwd *sp, खाता *stream) {
        निश्चित(sp);
        निश्चित(stream);

        त्रुटि_सं = 0;
        अगर (putspent(sp, stream) != 0)
                वापस errno_or_else(EIO);

        वापस 0;
}

पूर्णांक putgrent_sane(स्थिर काष्ठा group *gr, खाता *stream) {
        निश्चित(gr);
        निश्चित(stream);

        त्रुटि_सं = 0;
        अगर (putgrent(gr, stream) != 0)
                वापस errno_or_else(EIO);

        वापस 0;
}

#अगर ENABLE_GSHADOW
पूर्णांक putsgent_sane(स्थिर काष्ठा sgrp *sg, खाता *stream) {
        निश्चित(sg);
        निश्चित(stream);

        त्रुटि_सं = 0;
        अगर (putsgent(sg, stream) != 0)
                वापस errno_or_else(EIO);

        वापस 0;
}
#पूर्ण_अगर

पूर्णांक fgetpwent_sane(खाता *stream, काष्ठा passwd **pw) {
        निश्चित(stream);
        निश्चित(pw);

        त्रुटि_सं = 0;
        काष्ठा passwd *p = fgetpwent(stream);
        अगर (!p && !IN_SET(त्रुटि_सं, 0, ENOENT))
                वापस -त्रुटि_सं;

        *pw = p;
        वापस !!p;
}

पूर्णांक fgetspent_sane(खाता *stream, काष्ठा spwd **sp) {
        निश्चित(stream);
        निश्चित(sp);

        त्रुटि_सं = 0;
        काष्ठा spwd *s = fgetspent(stream);
        अगर (!s && !IN_SET(त्रुटि_सं, 0, ENOENT))
                वापस -त्रुटि_सं;

        *sp = s;
        वापस !!s;
}

पूर्णांक fgetgrent_sane(खाता *stream, काष्ठा group **gr) {
        निश्चित(stream);
        निश्चित(gr);

        त्रुटि_सं = 0;
        काष्ठा group *g = fgetgrent(stream);
        अगर (!g && !IN_SET(त्रुटि_सं, 0, ENOENT))
                वापस -त्रुटि_सं;

        *gr = g;
        वापस !!g;
}

#अगर ENABLE_GSHADOW
पूर्णांक fgetsgent_sane(खाता *stream, काष्ठा sgrp **sg) {
        निश्चित(stream);
        निश्चित(sg);

        त्रुटि_सं = 0;
        काष्ठा sgrp *s = fgetsgent(stream);
        अगर (!s && !IN_SET(त्रुटि_सं, 0, ENOENT))
                वापस -त्रुटि_सं;

        *sg = s;
        वापस !!s;
}
#पूर्ण_अगर

पूर्णांक is_this_me(स्थिर अक्षर *username) {
        uid_t uid;
        पूर्णांक r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, शून्य, &uid, शून्य, शून्य, शून्य);
        अगर (r < 0)
                वापस r;

        वापस uid == getuid();
}

स्थिर अक्षर* get_home_मूल(व्योम) {
        /* For debug purposes allow overriding where we look for home dirs */
        स्थिर अक्षर *e = secure_getenv("SYSTEMD_HOME_ROOT");
        अगर (e && path_is_absolute(e) && path_is_normalized(e))
                वापस e;

        वापस "/home";
}

अटल पूर्णांक copy_struct_passwd(स्थिर काष्ठा passwd *pw, काष्ठा passwd **ret) {
        निश्चित(pw);
        निश्चित(ret);

        माप_प्रकार need_bytes = माप(काष्ठा passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        अक्षर *buf = दो_स्मृति(need_bytes);
        अगर (!buf)
                वापस -ENOMEM;

        काष्ठा passwd *newpw = (काष्ठा passwd *) buf;

        /* The layout in our buffer:
         * struct passwd, and then individual strings. */
        अक्षर *p = buf + माप(काष्ठा passwd);

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
        वापस 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
पूर्णांक lookup_pwent_in_files(
                अक्षर * स्थिर *files,
                स्थिर अक्षर *name,
                uid_t uid,
                काष्ठा passwd **ret) {

        पूर्णांक r;

        निश्चित(files);
        निश्चित(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ खाता *f = शून्य;
                काष्ठा passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                अगर (r == -ENOENT)
                        जारी;
                अगर (r < 0)
                        वापस r;

                जबतक ((r = fgetpwent_sane(f, &pw)) > 0) {
                        अगर (name && !streq_ptr(pw->pw_name, name))
                                जारी;
                        अगर (uid_is_valid(uid) && pw->pw_uid != uid)
                                जारी;
                        अगर (ret)
                                वापस copy_struct_passwd(pw, ret);
                        वापस 0;
                }
                अगर (r < 0)
                        वापस r;
        }

        वापस -ESRCH;
}

#अगर !BUILD_STATIC

अटल माप_प्रकार getpw_बफर_आकार(व्योम) {
        दीर्घ bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        वापस bufsize <= 0 ? 4096U : (माप_प्रकार) bufsize;
}

अटल bool errno_is_user_doesnt_exist(पूर्णांक error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        वापस IN_SET(असल(error), ENOENT, ESRCH, EBADF, EPERM);
}

#पूर्ण_अगर

पूर्णांक getpwnam_malloc(स्थिर अक्षर *name, काष्ठा passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        अगर (isempty(name))
                वापस -EINVAL;

#अगर BUILD_STATIC
        वापस lookup_pwent_in_files(PASSWD_FILES, name, UID_INVALID, ret);
#अन्यथा
        माप_प्रकार bufsize = getpw_बफर_आकार();
        पूर्णांक r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        क्रम (;;) {
                _cleanup_free_ व्योम *buf = शून्य;

                /* Silence static analyzers */
                निश्चित(bufsize <= SIZE_MAX - ALIGN(माप(काष्ठा passwd)));
                buf = malloc0(ALIGN(माप(काष्ठा passwd)) + bufsize);
                अगर (!buf)
                        वापस -ENOMEM;

                काष्ठा passwd *pw = शून्य;
                r = getpwnam_r(name, buf, (अक्षर*) buf + ALIGN(माप(काष्ठा passwd)), bufsize, &pw);
                अगर (r == 0) {
                        अगर (pw) {
                                अगर (ret)
                                        *ret = TAKE_PTR(buf);
                                वापस 0;
                        }

                        वापस -ESRCH;
                }

                निश्चित(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                अगर (errno_is_user_doesnt_exist(r))
                        वापस -ESRCH;
                अगर (r != दुस्फल)
                        वापस -r;

                अगर (bufsize > SIZE_MAX/2 - ALIGN(माप(काष्ठा passwd)))
                        वापस -ENOMEM;
                bufsize *= 2;
        }
#पूर्ण_अगर
}

पूर्णांक getpwuid_malloc(uid_t uid, काष्ठा passwd **ret) {
        अगर (!uid_is_valid(uid))
                वापस -EINVAL;

#अगर BUILD_STATIC
        वापस lookup_pwent_in_files(PASSWD_FILES, /* name= */ शून्य, uid, ret);
#अन्यथा
        माप_प्रकार bufsize = getpw_बफर_आकार();
        पूर्णांक r;

        क्रम (;;) {
                _cleanup_free_ व्योम *buf = शून्य;

                /* Silence static analyzers */
                निश्चित(bufsize <= SIZE_MAX - ALIGN(माप(काष्ठा passwd)));
                buf = malloc0(ALIGN(माप(काष्ठा passwd)) + bufsize);
                अगर (!buf)
                        वापस -ENOMEM;

                काष्ठा passwd *pw = शून्य;
                r = getpwuid_r(uid, buf, (अक्षर*) buf + ALIGN(माप(काष्ठा passwd)), bufsize, &pw);
                अगर (r == 0) {
                        अगर (pw) {
                                अगर (ret)
                                        *ret = TAKE_PTR(buf);
                                वापस 0;
                        }

                        वापस -ESRCH;
                }

                निश्चित(r > 0);

                अगर (errno_is_user_doesnt_exist(r))
                        वापस -ESRCH;
                अगर (r != दुस्फल)
                        वापस -r;

                अगर (bufsize > SIZE_MAX/2 - ALIGN(माप(काष्ठा passwd)))
                        वापस -ENOMEM;
                bufsize *= 2;
        }
#पूर्ण_अगर
}

अटल पूर्णांक copy_struct_group(स्थिर काष्ठा group *gr, काष्ठा group **ret) {
        निश्चित(gr);
        निश्चित(ret);

        माप_प्रकार need_bytes = माप(काष्ठा group)
                + strlen_ptr(gr->gr_name) + 1
                + strlen_ptr(gr->gr_passwd) + 1,
                n_mem = 0;
        STRV_FOREACH(s, gr->gr_mem) {
                need_bytes += माप(अक्षर*) + म_माप(*s) + 1;
                n_mem++;
        }
        need_bytes += माप(अक्षर*);  /* NULL terminator for gr_mem */

        अक्षर *buf = दो_स्मृति(need_bytes);
        अगर (!buf)
                वापस -ENOMEM;

        काष्ठा group *newgr = (काष्ठा group *) buf;

        /* The layout in our buffer:
         * struct group, ->gr_mem pointers terminated by NULL, ->gr_name, ->gr_passwd, ->gr_mem items */
        /* The ->gr_mem array is first, because it needs alignment. */
        assert_cc(माप(काष्ठा group) % alignof(अक्षर*) == 0);

        अक्षर *p = buf + माप(काष्ठा group) + (n_mem + 1) * माप(अक्षर*);

        newgr->gr_name = p;
        p = stpcpy(p, strempty(gr->gr_name)) + 1;

        newgr->gr_passwd = p;
        p = stpcpy(p, strempty(gr->gr_passwd)) + 1;

        newgr->gr_gid = gr->gr_gid;

        newgr->gr_mem = (अक्षर**) (buf + माप(काष्ठा group));
        क्रम (माप_प्रकार i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = शून्य;

        *ret = newgr;
        वापस 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
पूर्णांक lookup_grent_in_files(
                अक्षर * स्थिर *files,
                स्थिर अक्षर *name,
                gid_t gid,
                काष्ठा group **ret) {

        पूर्णांक r;

        निश्चित(files);
        निश्चित(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ खाता *f = शून्य;
                काष्ठा group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                अगर (r == -ENOENT)
                        जारी;
                अगर (r < 0)
                        वापस r;

                जबतक ((r = fgetgrent_sane(f, &gr)) > 0) {
                        अगर (name && !streq_ptr(gr->gr_name, name))
                                जारी;
                        अगर (gid_is_valid(gid) && gr->gr_gid != gid)
                                जारी;
                        अगर (ret)
                                वापस copy_struct_group(gr, ret);
                        वापस 0;
                }
                अगर (r < 0)
                        वापस r;
        }

        वापस -ESRCH;
}

पूर्णांक sysconf_ngroups_max(व्योम) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        त्रुटि_सं = 0;
        दीर्घ ngroups_max = sysconf(_SC_NGROUPS_MAX);
        अगर (ngroups_max <= 0)
                वापस errno_or_else(EOPNOTSUPP);
        अगर (ngroups_max > पूर्णांक_उच्च)
                वापस -दुस्फल;
        वापस ngroups_max;
}

ssize_t lookup_groups_in_files(
                अक्षर * स्थिर *files,
                स्थिर अक्षर *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = शून्य;
        ssize_t n_arr = 0;
        पूर्णांक r;

        निश्चित(files);
        निश्चित(name);
        निश्चित(gid_is_valid(gid));

        पूर्णांक ngroups_max = sysconf_ngroups_max();
        अगर (ngroups_max < 0)
                वापस ngroups_max;

        अगर (!GREEDY_REALLOC(arr, n_arr + 1))
                वापस -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ खाता *f = शून्य;
                काष्ठा group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                अगर (r == -ENOENT)
                        जारी;
                अगर (r < 0)
                        वापस r;

                जबतक ((r = fgetgrent_sane(f, &gr)) > 0) {
                        अगर (gid == gr->gr_gid)
                                जारी;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        अगर (gid_list_has(arr, n_arr, gr->gr_gid))
                                जारी;

                        अगर (!strv_contains(gr->gr_mem, name))
                                जारी;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        अगर (n_arr >= ngroups_max)
                                वापस -ENOBUFS;

                        अगर (!GREEDY_REALLOC(arr, n_arr + 1))
                                वापस -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                अगर (r < 0)
                        वापस r;
        }

        अगर (ret)
                *ret = TAKE_PTR(arr);
        वापस n_arr;
}

पूर्णांक getgrouplist_malloc(स्थिर अक्षर *user, gid_t gid, gid_t **ret) {
#अगर BUILD_STATIC
        वापस lookup_groups_in_files(GROUP_FILES, user, gid, ret);
#अन्यथा
        पूर्णांक ngroups_max = sysconf_ngroups_max();
        अगर (ngroups_max < 0)
                वापस ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        अगर (!gids)
                वापस -ENOMEM;

        पूर्णांक k = ngroups_max;
        अगर (getgrouplist(user, gid, gids, &k) < 0)
                वापस -EIO;  /* getgrouplist does not set errno */

        अगर (ret)
                *ret = TAKE_PTR(gids);
        वापस k;
#पूर्ण_अगर
}

पूर्णांक initgroups_wrapper(स्थिर अक्षर *user, gid_t gid) {
#अगर BUILD_STATIC
        _cleanup_free_ gid_t *groups = शून्य;
        पूर्णांक r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        अगर (n_groups <= 0)
                वापस n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        क्रम (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                अगर (r != -EINVAL)
                        अवरोध;
        }

        वापस r;
#अन्यथा
        वापस RET_NERRNO(initgroups(user, gid));
#पूर्ण_अगर
}

#अगर !BUILD_STATIC

अटल माप_प्रकार getgr_बफर_आकार(व्योम) {
        दीर्घ bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        वापस bufsize <= 0 ? 4096U : (माप_प्रकार) bufsize;
}

#पूर्ण_अगर

पूर्णांक getgrnam_malloc(स्थिर अक्षर *name, काष्ठा group **ret) {
        अगर (isempty(name))
                वापस -EINVAL;

#अगर BUILD_STATIC
        वापस lookup_grent_in_files(GROUP_FILES, name, GID_INVALID, ret);
#अन्यथा
        माप_प्रकार bufsize = getgr_बफर_आकार();
        पूर्णांक r;

        क्रम (;;) {
                _cleanup_free_ व्योम *buf = शून्य;

                /* Silence static analyzers */
                निश्चित(bufsize <= SIZE_MAX - ALIGN(माप(काष्ठा group)));
                buf = malloc0(ALIGN(माप(काष्ठा group)) + bufsize);
                अगर (!buf)
                        वापस -ENOMEM;

                काष्ठा group *gr = शून्य;
                r = getgrnam_r(name, buf, (अक्षर*) buf + ALIGN(माप(काष्ठा group)), bufsize, &gr);
                अगर (r == 0) {
                        अगर (gr) {
                                अगर (ret)
                                        *ret = TAKE_PTR(buf);
                                वापस 0;
                        }

                        वापस -ESRCH;
                }

                निश्चित(r > 0);

                अगर (errno_is_user_doesnt_exist(r))
                        वापस -ESRCH;
                अगर (r != दुस्फल)
                        वापस -r;

                अगर (bufsize > SIZE_MAX/2 - ALIGN(माप(काष्ठा group)))
                        वापस -ENOMEM;
                bufsize *= 2;
        }
#पूर्ण_अगर
}

पूर्णांक getgrgid_malloc(gid_t gid, काष्ठा group **ret) {
        अगर (!gid_is_valid(gid))
                वापस -EINVAL;

#अगर BUILD_STATIC
        वापस lookup_grent_in_files(GROUP_FILES, /* name= */ शून्य, gid, ret);
#अन्यथा
        माप_प्रकार bufsize = getgr_बफर_आकार();
        पूर्णांक r;

        क्रम (;;) {
                _cleanup_free_ व्योम *buf = शून्य;

                /* Silence static analyzers */
                निश्चित(bufsize <= SIZE_MAX - ALIGN(माप(काष्ठा group)));
                buf = malloc0(ALIGN(माप(काष्ठा group)) + bufsize);
                अगर (!buf)
                        वापस -ENOMEM;

                काष्ठा group *gr = शून्य;
                r = getgrgid_r(gid, buf, (अक्षर*) buf + ALIGN(माप(काष्ठा group)), bufsize, &gr);
                अगर (r == 0) {
                        अगर (gr) {
                                अगर (ret)
                                        *ret = TAKE_PTR(buf);
                                वापस 0;
                        }

                        वापस -ESRCH;
                }

                निश्चित(r > 0);

                अगर (errno_is_user_doesnt_exist(r))
                        वापस -ESRCH;
                अगर (r != दुस्फल)
                        वापस -r;

                अगर (bufsize > SIZE_MAX/2 - ALIGN(माप(काष्ठा group)))
                        वापस -ENOMEM;
                bufsize *= 2;
        }
#पूर्ण_अगर
}
