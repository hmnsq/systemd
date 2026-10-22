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
        ध्रुव वर्ण* strerror_##type(                                    \
                        पूर्णाङ्क errnum,                                     \
                        वर्ण *buf,                                      \
                        माप_प्रकार buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                विकल्प (errnum) {                                       \
                पक्ष ESRCH:                                             \
                        प्रत्यागम "Unknown " STRINGIFY(type);              \
                पक्ष ENOEXEC:                                           \
                        प्रत्यागम "Not a system " STRINGIFY(type);         \
                स्वभाव:                                                \
                        प्रत्यागम strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        यदि (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                प्रत्यागम false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        यदि (uid == (uid_t) UINT32_C(0xFFFF))
                प्रत्यागम false;

        प्रत्यागम true;
}

पूर्णाङ्क parse_uid(ध्रुव वर्ण *s, uid_t *ret) {
        uint32_t uid = 0;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        assert_cc(परिमाण(uid_t) == परिमाण(uint32_t));

        /* We are very strict when parsing UIDs, and prohibit +/- as prefix, leading zero as prefix, and
         * whitespace. We do this, since this call is often used in a context where we parse things as UID
         * first, and if that doesn't work we fall back to NSS. Thus we really want to make sure that UIDs
         * are parsed as UIDs only if they really really look like UIDs. */
        r = safe_atou32_full(s, 10
                             | SAFE_ATO_REFUSE_PLUS_MINUS
                             | SAFE_ATO_REFUSE_LEADING_ZERO
                             | SAFE_ATO_REFUSE_LEADING_WHITESPACE, &uid);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (!uid_is_valid(uid))
                प्रत्यागम -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        यदि (ret)
                *ret = uid;

        प्रत्यागम 0;
}

पूर्णाङ्क parse_uid_range(ध्रुव वर्ण *s, uid_t *ret_lower, uid_t *ret_upper) {
        _cleanup_free_ वर्ण *word = रिक्त;
        uid_t l, u;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret_lower);
        प्रतिज्ञा(ret_upper);

        r = extract_first_word(&s, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r == 0)
                प्रत्यागम -EINVAL;

        r = parse_uid(word, &l);
        यदि (r < 0)
                प्रत्यागम r;

        /* Check for the upper bound and extract it if needed */
        यदि (!s)
                /* Single number with no dash. */
                u = l;
        अन्यथा यदि (!*s)
                /* Trailing dash is an error. */
                प्रत्यागम -EINVAL;
        अन्यथा {
                r = parse_uid(s, &u);
                यदि (r < 0)
                        प्रत्यागम r;

                यदि (l > u)
                        प्रत्यागम -EINVAL;
        }

        *ret_lower = l;
        *ret_upper = u;
        प्रत्यागम 0;
}

वर्ण* getlogname_malloc(शून्य) {
        uid_t uid;
        संरचना stat st;

        यदि (isatty_safe(STDIN_FILENO) && ख_स्थिति(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        अन्यथा
                uid = getuid();

        प्रत्यागम uid_to_name(uid);
}

वर्ण* getusername_malloc(शून्य) {
        ध्रुव वर्ण *e;

        e = secure_getenv("USER");
        यदि (e)
                प्रत्यागम strdup(e);

        प्रत्यागम uid_to_name(getuid());
}

bool is_nologin_shell(ध्रुव वर्ण *shell) {
        प्रत्यागम PATH_IN_SET(shell,
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

bool shell_is_placeholder(ध्रुव वर्ण *shell) {
        प्रत्यागम isempty(shell) || is_nologin_shell(shell);
}

ध्रुव वर्ण* default_मूल_shell_at(पूर्णाङ्क rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        प्रतिज्ञा(rfd >= 0 || rfd == AT_FDCWD);

        पूर्णाङ्क r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, रिक्त, रिक्त);
        यदि (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        यदि (r > 0)
                प्रत्यागम DEFAULT_USER_SHELL;

        प्रत्यागम "/bin/sh";
}

ध्रुव वर्ण* default_मूल_shell(ध्रुव वर्ण *मूल) {
        _cleanup_close_ पूर्णाङ्क rfd = -EBADF;

        rfd = खोलो(empty_to_मूल(मूल), O_CLOEXEC | O_DIRECTORY | O_PATH);
        यदि (rfd < 0)
                प्रत्यागम "/bin/sh";

        प्रत्यागम default_मूल_shell_at(rfd);
}

स्थिर पूर्णाङ्क return_user_creds(
                ध्रुव वर्ण *username,
                uid_t uid, gid_t gid,
                ध्रुव वर्ण *home,
                ध्रुव वर्ण *shell,
                वर्ण **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                वर्ण **ret_home,
                वर्ण **ret_shell) {
        /* Helper function to help with the strdups and atomic setting of return params. */

        _cleanup_free_ वर्ण *s1 = रिक्त, *s2 = रिक्त, *s3 = रिक्त;
        पूर्णाङ्क r;

        यदि (ret_username) {
                r = strdup_to(&s1, username);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        यदि (ret_home) {
                r = strdup_to(&s2, home);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        यदि (ret_shell) {
                r = strdup_to(&s3, shell);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        यदि (ret_username)
                *ret_username = TAKE_PTR(s1);
        यदि (ret_uid)
                *ret_uid = uid;
        यदि (ret_gid)
                *ret_gid = gid;
        यदि (ret_home)
                *ret_home = TAKE_PTR(s2);
        यदि (ret_shell)
                *ret_shell = TAKE_PTR(s3);
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क synthesize_user_creds(
                ध्रुव वर्ण *username,
                UserCredsFlags flags,
                वर्ण **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                वर्ण **ret_home,
                वर्ण **ret_shell) {
        प्रतिज्ञा(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        यदि (STR_IN_SET(username, "root", "0"))
                प्रत्यागम return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_मूल_shell(रिक्त) : रिक्त,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        यदि (STR_IN_SET(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                प्रत्यागम return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? रिक्त : "/",
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? रिक्त : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        प्रत्यागम -ENOMEDIUM;
}

पूर्णाङ्क get_user_creds(
                ध्रुव वर्ण *username,
                UserCredsFlags flags,
                वर्ण **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                वर्ण **ret_home,
                वर्ण **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ संरचना passwd *pw = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(username);
        प्रतिज्ञा((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        यदि (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS) ||
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
                यदि (r >= 0)
                        प्रत्यागम 0;
                यदि (r != -ENOMEDIUM) /* not a username we can synthesize */
                        प्रत्यागम r;
        }

        यदि (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                यदि (r >= 0)
                        username = pw->pw_name;

                अन्यथा यदि (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        यदि (ret_username)
                                *ret_username = रिक्त;
                        यदि (ret_uid)
                                *ret_uid = u;
                        प्रत्यागम 0;
                }
        } अन्यथा
                r = getpwnam_malloc(username, &pw);

        यदि (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                यदि (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        प्रत्यागम 0;

                प्रत्यागम r;
        }

        यदि (ret_uid && !uid_is_valid(pw->pw_uid))
                प्रत्यागम -EBADMSG;

        यदि (ret_gid && !gid_is_valid(pw->pw_gid))
                प्रत्यागम -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        ध्रुव वर्ण *h =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && empty_or_मूल(pw->pw_dir)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_dir) || !path_is_absolute(pw->pw_dir)))
                ? रिक्त : pw->pw_dir;

        ध्रुव वर्ण *s =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_shell) || !path_is_absolute(pw->pw_shell)))
                ? रिक्त : pw->pw_shell;

        प्रत्यागम return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

स्थिर पूर्णाङ्क synthesize_group_creds(ध्रुव वर्ण *groupname, वर्ण **ret_name, gid_t *ret_gid) {
        प्रतिज्ञा(groupname);

        gid_t id;
        ध्रुव वर्ण *n;
        पूर्णाङ्क r;

        यदि (STR_IN_SET(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } अन्यथा यदि (STR_IN_SET(groupname, NOBODY_GROUP_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_GROUP_NAME;
        } अन्यथा
                प्रत्यागम -ENOMEDIUM;

        r = strdup_to_full(ret_name, n);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (ret_gid)
                *ret_gid = id;
        प्रत्यागम 0;
}

पूर्णाङ्क get_group_creds(ध्रुव वर्ण *groupname, UserCredsFlags flags, वर्ण **ret_name, gid_t *ret_gid) {
        _cleanup_free_ संरचना group *gr = रिक्त;
        gid_t id;
        पूर्णाङ्क r;

        प्रतिज्ञा(groupname);

        यदि (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_group_creds(groupname, ret_name, ret_gid);
                यदि (r >= 0)
                        प्रत्यागम 0;
                यदि (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        प्रत्यागम r;
        }

        यदि (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                यदि (r >= 0)
                        groupname = gr->gr_name;
                अन्यथा यदि (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING)) {
                        यदि (ret_gid)
                                *ret_gid = id;
                        यदि (ret_name)
                                *ret_name = रिक्त;
                        प्रत्यागम 0;
                }
        } अन्यथा
                r = getgrnam_malloc(groupname, &gr);

        यदि (r < 0) {
                यदि (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_group_creds(groupname, ret_name, ret_gid) >= 0)
                        प्रत्यागम 0;
                प्रत्यागम r;
        }

        यदि (ret_gid && !gid_is_valid(gr->gr_gid))
                प्रत्यागम -EBADMSG;

        r = strdup_to_full(ret_name, groupname);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (ret_gid)
                *ret_gid = gr->gr_gid;
        प्रत्यागम 0;
}

वर्ण* uid_to_name(uid_t uid) {
        वर्ण *ret;
        पूर्णाङ्क r;

        /* Shortcut things to avoid NSS lookups */
        यदि (uid == 0)
                प्रत्यागम strdup("root");
        यदि (uid == UID_NOBODY && synthesize_nobody())
                प्रत्यागम strdup(NOBODY_USER_NAME);

        यदि (uid_is_valid(uid)) {
                _cleanup_free_ संरचना passwd *pw = रिक्त;

                r = getpwuid_malloc(uid, &pw);
                यदि (r >= 0)
                        प्रत्यागम strdup(pw->pw_name);
        }

        यदि (asprintf(&ret, UID_FMT, uid) < 0)
                प्रत्यागम रिक्त;

        प्रत्यागम ret;
}

वर्ण* gid_to_name(gid_t gid) {
        वर्ण *ret;
        पूर्णाङ्क r;

        यदि (gid == 0)
                प्रत्यागम strdup("root");
        यदि (gid == GID_NOBODY && synthesize_nobody())
                प्रत्यागम strdup(NOBODY_GROUP_NAME);

        यदि (gid_is_valid(gid)) {
                _cleanup_free_ संरचना group *gr = रिक्त;

                r = getgrgid_malloc(gid, &gr);
                यदि (r >= 0)
                        प्रत्यागम strdup(gr->gr_name);
        }

        यदि (asprintf(&ret, GID_FMT, gid) < 0)
                प्रत्यागम रिक्त;

        प्रत्यागम ret;
}

स्थिर bool gid_list_has(ध्रुव gid_t *list, माप_प्रकार size, gid_t val) {
        प्रतिज्ञा(list || size == 0);

        FOREACH_ARRAY(i, list, size)
                यदि (*i == val)
                        प्रत्यागम true;

        प्रत्यागम false;
}

पूर्णाङ्क in_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = रिक्त;
        पूर्णाङ्क ngroups;

        यदि (getgid() == gid)
                प्रत्यागम 1;

        यदि (getegid() == gid)
                प्रत्यागम 1;

        यदि (!gid_is_valid(gid))
                प्रत्यागम -EINVAL;

        ngroups = getgroups_alloc(&gids);
        यदि (ngroups < 0)
                प्रत्यागम ngroups;

        प्रत्यागम gid_list_has(gids, ngroups, gid);
}

पूर्णाङ्क in_group(ध्रुव वर्ण *name) {
        पूर्णाङ्क r;
        gid_t gid;

        r = get_group_creds(name, /* flags= */ 0, /* ret_name= */ रिक्त, &gid);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम in_gid(gid);
}

पूर्णाङ्क merge_gid_lists(ध्रुव gid_t *list1, माप_प्रकार size1, ध्रुव gid_t *list2, माप_प्रकार size2, gid_t **ret) {
        माप_प्रकार nresult = 0;

        प्रतिज्ञा(size1 == 0 || list1);
        प्रतिज्ञा(size2 == 0 || list2);
        प्रतिज्ञा(ret);

        यदि (size2 > पूर्णांक_उच्च - size1)
                प्रत्यागम -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        यदि (!buf)
                प्रत्यागम -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        प्रति (माप_प्रकार i = 0; i < size1; i++)
                यदि (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        प्रति (माप_प्रकार i = 0; i < size2; i++)
                यदि (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        प्रत्यागम (पूर्णाङ्क)nresult;
}

पूर्णाङ्क getgroups_alloc(gid_t **ret) {
        पूर्णाङ्क ngroups = 8;

        प्रतिज्ञा(ret);

        प्रति (अचिन्हित attempt = 0;;) {
                _cleanup_free_ gid_t *p = रिक्त;

                p = new(gid_t, ngroups);
                यदि (!p)
                        प्रत्यागम -ENOMEM;

                ngroups = getgroups(ngroups, p);
                यदि (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        प्रत्यागम ngroups;
                }
                यदि (ngroups == 0)
                        भङ्ग;
                यदि (त्रुटि_सं != EINVAL)
                        प्रत्यागम -त्रुटि_सं;

                /* Give up eventually */
                यदि (attempt++ > 10)
                        प्रत्यागम -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, रिक्त);
                यदि (ngroups < 0)
                        प्रत्यागम -त्रुटि_सं;
                यदि (ngroups == 0)
                        भङ्ग;
        }

        *ret = रिक्त;
        प्रत्यागम 0;
}

पूर्णाङ्क get_home_dir(वर्ण **ret) {
        _cleanup_free_ संरचना passwd *p = रिक्त;
        ध्रुव वर्ण *e;
        uid_t u;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        यदि (e && path_is_valid(e) && path_is_absolute(e))
                गच्छ found;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        यदि (u == 0) {
                e = "/root";
                गच्छ found;
        }
        यदि (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                गच्छ found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        यदि (r < 0)
                प्रत्यागम r;

        e = p->pw_dir;
        यदि (!path_is_valid(e) || !path_is_absolute(e))
                प्रत्यागम -EINVAL;

 found:
        प्रत्यागम path_simplify_alloc(e, ret);
}

पूर्णाङ्क get_shell(वर्ण **ret) {
        _cleanup_free_ संरचना passwd *p = रिक्त;
        ध्रुव वर्ण *e;
        uid_t u;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        यदि (e && path_is_valid(e) && path_is_absolute(e))
                गच्छ found;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        यदि (u == 0) {
                e = default_मूल_shell(रिक्त);
                गच्छ found;
        }
        यदि (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                गच्छ found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        यदि (r < 0)
                प्रत्यागम r;

        e = p->pw_shell;
        यदि (!path_is_valid(e) || !path_is_absolute(e))
                प्रत्यागम -EINVAL;

 found:
        प्रत्यागम path_simplify_alloc(e, ret);
}

पूर्णाङ्क fully_set_uid_gid(uid_t uid, gid_t gid, ध्रुव gid_t supplementary_gids[], माप_प्रकार n_supplementary_gids) {
        पूर्णाङ्क r;

        प्रतिज्ञा(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (gid_is_valid(gid))
                यदि (setresgid(gid, gid, gid) < 0)
                        प्रत्यागम -त्रुटि_सं;

        यदि (uid_is_valid(uid))
                यदि (setresuid(uid, uid, uid) < 0)
                        प्रत्यागम -त्रुटि_सं;

        प्रत्यागम 0;
}

पूर्णाङ्क take_etc_passwd_lock(ध्रुव वर्ण *मूल) {
        पूर्णाङ्क r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ वर्ण *path = path_join(मूल, ETC_PASSWD_LOCK_PATH);
        यदि (!path)
                प्रत्यागम log_oom_debug();

        (शून्य) mkdir_parents(path, 0755);

        _cleanup_close_ पूर्णाङ्क fd = खोलो(path, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        यदि (fd < 0)
                प्रत्यागम log_debug_errno(त्रुटि_सं, "Cannot open %s: %m", path);

        r = unposix_lock(fd, LOCK_EX);
        यदि (r < 0)
                प्रत्यागम log_debug_errno(r, "Locking %s failed: %m", path);

        प्रत्यागम TAKE_FD(fd);
}

bool valid_user_group_name(ध्रुव वर्ण *u, ValidUserFlags flags) {
        ध्रुव वर्ण *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        यदि (isempty(u)) /* An empty user name is never valid */
                प्रत्यागम false;

        यदि (parse_uid(u, रिक्त) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                प्रत्यागम FLAGS_SET(flags, VALID_USER_ALLOW_NUMERIC);

        यदि (FLAGS_SET(flags, VALID_USER_RELAX)) {

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

                यदि (startswith(u, " ") || endswith(u, " ")) /* At least expect whitespace padding is removed
                                                             * at front and back (accept in the middle, since
                                                             * that's apparently a thing on Windows). Note
                                                             * that this also blocks usernames consisting of
                                                             * whitespace only. */
                        प्रत्यागम false;

                यदि (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        प्रत्यागम false;

                यदि (string_has_cc(u, रिक्त)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        प्रत्यागम false;

                यदि (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        प्रत्यागम false;

                यदि (in_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        प्रत्यागम false;

                यदि (u[0] == '-' && in_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        प्रत्यागम false;

                यदि (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        प्रत्यागम false;

                /* Compare with strict result and warn if result doesn't match */
                यदि (FLAGS_SET(flags, VALID_USER_WARN) && !valid_user_group_name(u, 0))
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

                यदि (!ascii_isalpha(u[0]) &&
                    u[0] != '_')
                        प्रत्यागम false;

                प्रति (i = u+1; *i; i++)
                        यदि (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !IN_SET(*i, '_', '-'))
                                प्रत्यागम false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                यदि (l > (माप_प्रकार) sz) /* glibc: 256 */
                        प्रत्यागम false;
                यदि (l > NAME_MAX) /* must fit in a filename: 255 */
                        प्रत्यागम false;
                यदि (l > sizeof_field(संरचना utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        प्रत्यागम false;
        }

        प्रत्यागम true;
}

bool valid_gecos(ध्रुव वर्ण *d) {

        यदि (!d)
                प्रत्यागम false;

        यदि (!utf8_is_valid(d))
                प्रत्यागम false;

        यदि (string_has_cc(d, रिक्त))
                प्रत्यागम false;

        /* Colons are used as field separators, and hence not OK */
        यदि (म_अक्षर(d, ':'))
                प्रत्यागम false;

        प्रत्यागम true;
}

वर्ण* mangle_gecos(ध्रुव वर्ण *d) {
        वर्ण *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        यदि (!mangled)
                प्रत्यागम रिक्त;

        प्रति (वर्ण *i = mangled; *i; i++) {
                पूर्णाङ्क len;

                यदि ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        अनुवर्त;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                यदि (len < 0) {
                        *i = ' ';
                        अनुवर्त;
                }

                i += len - 1;
        }

        प्रत्यागम mangled;
}

bool valid_home(ध्रुव वर्ण *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        यदि (isempty(p))
                प्रत्यागम false;

        यदि (!utf8_is_valid(p))
                प्रत्यागम false;

        यदि (string_has_cc(p, रिक्त))
                प्रत्यागम false;

        यदि (!path_is_absolute(p))
                प्रत्यागम false;

        यदि (!path_is_normalized(p))
                प्रत्यागम false;

        /* Colons are used as field separators, and hence not OK */
        यदि (म_अक्षर(p, ':'))
                प्रत्यागम false;

        प्रत्यागम true;
}

bool valid_shell(ध्रुव वर्ण *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        यदि (!valid_home(p))
                प्रत्यागम false;

        प्रत्यागम !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

पूर्णाङ्क maybe_setgroups(माप_प्रकार size, ध्रुव gid_t *list) {
        पूर्णाङ्क r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        यदि (size == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ खाता *f = ख_खोलो("/proc/self/gid_map", "re");
                यदि (!f && त्रुटि_सं != ENOENT)
                        प्रत्यागम -त्रुटि_सं;
                यदि (f) {
                        r = safe_fgetc(f, /* ret= */ रिक्त);
                        यदि (r < 0)
                                प्रत्यागम r;
                        यदि (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                प्रत्यागम 0;
                        }
                }

                _cleanup_free_ वर्ण *setgroups_content = रिक्त;
                r = read_one_पंक्ति_file("/proc/self/setgroups", &setgroups_content);
                यदि (r < 0 && r != -ENOENT)
                        प्रत्यागम r;
                यदि (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        प्रत्यागम 0;
                }
        }

        प्रत्यागम RET_NERRNO(setgroups(size, list));
}

bool synthesize_nobody(शून्य) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        स्थिर पूर्णाङ्क cache = -1;

        यदि (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        प्रत्यागम cache;
}

पूर्णाङ्क putpwent_sane(ध्रुव संरचना passwd *pw, खाता *stream) {
        प्रतिज्ञा(pw);
        प्रतिज्ञा(stream);

        त्रुटि_सं = 0;
        यदि (putpwent(pw, stream) != 0)
                प्रत्यागम errno_or_else(EIO);

        प्रत्यागम 0;
}

पूर्णाङ्क putspent_sane(ध्रुव संरचना spwd *sp, खाता *stream) {
        प्रतिज्ञा(sp);
        प्रतिज्ञा(stream);

        त्रुटि_सं = 0;
        यदि (putspent(sp, stream) != 0)
                प्रत्यागम errno_or_else(EIO);

        प्रत्यागम 0;
}

पूर्णाङ्क putgrent_sane(ध्रुव संरचना group *gr, खाता *stream) {
        प्रतिज्ञा(gr);
        प्रतिज्ञा(stream);

        त्रुटि_सं = 0;
        यदि (putgrent(gr, stream) != 0)
                प्रत्यागम errno_or_else(EIO);

        प्रत्यागम 0;
}

#यदि ENABLE_GSHADOW
पूर्णाङ्क putsgent_sane(ध्रुव संरचना sgrp *sg, खाता *stream) {
        प्रतिज्ञा(sg);
        प्रतिज्ञा(stream);

        त्रुटि_सं = 0;
        यदि (putsgent(sg, stream) != 0)
                प्रत्यागम errno_or_else(EIO);

        प्रत्यागम 0;
}
#endif

पूर्णाङ्क fgetpwent_sane(खाता *stream, संरचना passwd **pw) {
        प्रतिज्ञा(stream);
        प्रतिज्ञा(pw);

        त्रुटि_सं = 0;
        संरचना passwd *p = fgetpwent(stream);
        यदि (!p && !IN_SET(त्रुटि_सं, 0, ENOENT))
                प्रत्यागम -त्रुटि_सं;

        *pw = p;
        प्रत्यागम !!p;
}

पूर्णाङ्क fgetspent_sane(खाता *stream, संरचना spwd **sp) {
        प्रतिज्ञा(stream);
        प्रतिज्ञा(sp);

        त्रुटि_सं = 0;
        संरचना spwd *s = fgetspent(stream);
        यदि (!s && !IN_SET(त्रुटि_सं, 0, ENOENT))
                प्रत्यागम -त्रुटि_सं;

        *sp = s;
        प्रत्यागम !!s;
}

पूर्णाङ्क fgetgrent_sane(खाता *stream, संरचना group **gr) {
        प्रतिज्ञा(stream);
        प्रतिज्ञा(gr);

        त्रुटि_सं = 0;
        संरचना group *g = fgetgrent(stream);
        यदि (!g && !IN_SET(त्रुटि_सं, 0, ENOENT))
                प्रत्यागम -त्रुटि_सं;

        *gr = g;
        प्रत्यागम !!g;
}

#यदि ENABLE_GSHADOW
पूर्णाङ्क fgetsgent_sane(खाता *stream, संरचना sgrp **sg) {
        प्रतिज्ञा(stream);
        प्रतिज्ञा(sg);

        त्रुटि_सं = 0;
        संरचना sgrp *s = fgetsgent(stream);
        यदि (!s && !IN_SET(त्रुटि_सं, 0, ENOENT))
                प्रत्यागम -त्रुटि_सं;

        *sg = s;
        प्रत्यागम !!s;
}
#endif

पूर्णाङ्क is_this_me(ध्रुव वर्ण *username) {
        uid_t uid;
        पूर्णाङ्क r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, रिक्त, &uid, रिक्त, रिक्त, रिक्त);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम uid == getuid();
}

ध्रुव वर्ण* get_home_मूल(शून्य) {
        /* For debug purposes allow overriding where we look for home dirs */
        ध्रुव वर्ण *e = secure_getenv("SYSTEMD_HOME_ROOT");
        यदि (e && path_is_absolute(e) && path_is_normalized(e))
                प्रत्यागम e;

        प्रत्यागम "/home";
}

स्थिर पूर्णाङ्क copy_struct_passwd(ध्रुव संरचना passwd *pw, संरचना passwd **ret) {
        प्रतिज्ञा(pw);
        प्रतिज्ञा(ret);

        माप_प्रकार need_bytes = परिमाण(संरचना passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        वर्ण *buf = दो_स्मृति(need_bytes);
        यदि (!buf)
                प्रत्यागम -ENOMEM;

        संरचना passwd *newpw = (संरचना passwd *) buf;

        /* The layout in our buffer:
         * struct passwd, and then individual strings. */
        वर्ण *p = buf + परिमाण(संरचना passwd);

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
        प्रत्यागम 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
पूर्णाङ्क lookup_pwent_in_files(
                वर्ण * ध्रुव *files,
                ध्रुव वर्ण *name,
                uid_t uid,
                संरचना passwd **ret) {

        पूर्णाङ्क r;

        प्रतिज्ञा(files);
        प्रतिज्ञा(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ खाता *f = रिक्त;
                संरचना passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                यदि (r == -ENOENT)
                        अनुवर्त;
                यदि (r < 0)
                        प्रत्यागम r;

                यावत् ((r = fgetpwent_sane(f, &pw)) > 0) {
                        यदि (name && !streq_ptr(pw->pw_name, name))
                                अनुवर्त;
                        यदि (uid_is_valid(uid) && pw->pw_uid != uid)
                                अनुवर्त;
                        यदि (ret)
                                प्रत्यागम copy_struct_passwd(pw, ret);
                        प्रत्यागम 0;
                }
                यदि (r < 0)
                        प्रत्यागम r;
        }

        प्रत्यागम -ESRCH;
}

#यदि !BUILD_STATIC

स्थिर माप_प्रकार getpw_buffer_size(शून्य) {
        दीर्घ bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        प्रत्यागम bufsize <= 0 ? 4096U : (माप_प्रकार) bufsize;
}

स्थिर bool errno_is_user_doesnt_exist(पूर्णाङ्क error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        प्रत्यागम IN_SET(असल(error), ENOENT, ESRCH, EBADF, EPERM);
}

#endif

पूर्णाङ्क getpwnam_malloc(ध्रुव वर्ण *name, संरचना passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        यदि (isempty(name))
                प्रत्यागम -EINVAL;

#यदि BUILD_STATIC
        प्रत्यागम lookup_pwent_in_files(PASSWD_FILES, name, UID_INVALID, ret);
#अन्यथा
        माप_प्रकार bufsize = getpw_buffer_size();
        पूर्णाङ्क r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        प्रति (;;) {
                _cleanup_free_ शून्य *buf = रिक्त;

                /* Silence static analyzers */
                प्रतिज्ञा(bufsize <= SIZE_MAX - ALIGN(परिमाण(संरचना passwd)));
                buf = malloc0(ALIGN(परिमाण(संरचना passwd)) + bufsize);
                यदि (!buf)
                        प्रत्यागम -ENOMEM;

                संरचना passwd *pw = रिक्त;
                r = getpwnam_r(name, buf, (वर्ण*) buf + ALIGN(परिमाण(संरचना passwd)), bufsize, &pw);
                यदि (r == 0) {
                        यदि (pw) {
                                यदि (ret)
                                        *ret = TAKE_PTR(buf);
                                प्रत्यागम 0;
                        }

                        प्रत्यागम -ESRCH;
                }

                प्रतिज्ञा(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                यदि (errno_is_user_doesnt_exist(r))
                        प्रत्यागम -ESRCH;
                यदि (r != दुस्फल)
                        प्रत्यागम -r;

                यदि (bufsize > SIZE_MAX/2 - ALIGN(परिमाण(संरचना passwd)))
                        प्रत्यागम -ENOMEM;
                bufsize *= 2;
        }
#endif
}

पूर्णाङ्क getpwuid_malloc(uid_t uid, संरचना passwd **ret) {
        यदि (!uid_is_valid(uid))
                प्रत्यागम -EINVAL;

#यदि BUILD_STATIC
        प्रत्यागम lookup_pwent_in_files(PASSWD_FILES, /* name= */ रिक्त, uid, ret);
#अन्यथा
        माप_प्रकार bufsize = getpw_buffer_size();
        पूर्णाङ्क r;

        प्रति (;;) {
                _cleanup_free_ शून्य *buf = रिक्त;

                /* Silence static analyzers */
                प्रतिज्ञा(bufsize <= SIZE_MAX - ALIGN(परिमाण(संरचना passwd)));
                buf = malloc0(ALIGN(परिमाण(संरचना passwd)) + bufsize);
                यदि (!buf)
                        प्रत्यागम -ENOMEM;

                संरचना passwd *pw = रिक्त;
                r = getpwuid_r(uid, buf, (वर्ण*) buf + ALIGN(परिमाण(संरचना passwd)), bufsize, &pw);
                यदि (r == 0) {
                        यदि (pw) {
                                यदि (ret)
                                        *ret = TAKE_PTR(buf);
                                प्रत्यागम 0;
                        }

                        प्रत्यागम -ESRCH;
                }

                प्रतिज्ञा(r > 0);

                यदि (errno_is_user_doesnt_exist(r))
                        प्रत्यागम -ESRCH;
                यदि (r != दुस्फल)
                        प्रत्यागम -r;

                यदि (bufsize > SIZE_MAX/2 - ALIGN(परिमाण(संरचना passwd)))
                        प्रत्यागम -ENOMEM;
                bufsize *= 2;
        }
#endif
}

स्थिर पूर्णाङ्क copy_struct_group(ध्रुव संरचना group *gr, संरचना group **ret) {
        प्रतिज्ञा(gr);
        प्रतिज्ञा(ret);

        माप_प्रकार need_bytes = परिमाण(संरचना group)
                + strlen_ptr(gr->gr_name) + 1
                + strlen_ptr(gr->gr_passwd) + 1,
                n_mem = 0;
        STRV_FOREACH(s, gr->gr_mem) {
                need_bytes += परिमाण(वर्ण*) + म_माप(*s) + 1;
                n_mem++;
        }
        need_bytes += परिमाण(वर्ण*);  /* NULL terminator for gr_mem */

        वर्ण *buf = दो_स्मृति(need_bytes);
        यदि (!buf)
                प्रत्यागम -ENOMEM;

        संरचना group *newgr = (संरचना group *) buf;

        /* The layout in our buffer:
         * struct group, ->gr_mem pointers terminated by NULL, ->gr_name, ->gr_passwd, ->gr_mem items */
        /* The ->gr_mem array is first, because it needs alignment. */
        assert_cc(परिमाण(संरचना group) % alignof(वर्ण*) == 0);

        वर्ण *p = buf + परिमाण(संरचना group) + (n_mem + 1) * परिमाण(वर्ण*);

        newgr->gr_name = p;
        p = stpcpy(p, strempty(gr->gr_name)) + 1;

        newgr->gr_passwd = p;
        p = stpcpy(p, strempty(gr->gr_passwd)) + 1;

        newgr->gr_gid = gr->gr_gid;

        newgr->gr_mem = (वर्ण**) (buf + परिमाण(संरचना group));
        प्रति (माप_प्रकार i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = रिक्त;

        *ret = newgr;
        प्रत्यागम 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
पूर्णाङ्क lookup_grent_in_files(
                वर्ण * ध्रुव *files,
                ध्रुव वर्ण *name,
                gid_t gid,
                संरचना group **ret) {

        पूर्णाङ्क r;

        प्रतिज्ञा(files);
        प्रतिज्ञा(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ खाता *f = रिक्त;
                संरचना group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                यदि (r == -ENOENT)
                        अनुवर्त;
                यदि (r < 0)
                        प्रत्यागम r;

                यावत् ((r = fgetgrent_sane(f, &gr)) > 0) {
                        यदि (name && !streq_ptr(gr->gr_name, name))
                                अनुवर्त;
                        यदि (gid_is_valid(gid) && gr->gr_gid != gid)
                                अनुवर्त;
                        यदि (ret)
                                प्रत्यागम copy_struct_group(gr, ret);
                        प्रत्यागम 0;
                }
                यदि (r < 0)
                        प्रत्यागम r;
        }

        प्रत्यागम -ESRCH;
}

पूर्णाङ्क sysconf_ngroups_max(शून्य) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        त्रुटि_सं = 0;
        दीर्घ ngroups_max = sysconf(_SC_NGROUPS_MAX);
        यदि (ngroups_max <= 0)
                प्रत्यागम errno_or_else(EOPNOTSUPP);
        यदि (ngroups_max > पूर्णांक_उच्च)
                प्रत्यागम -दुस्फल;
        प्रत्यागम ngroups_max;
}

ssize_t lookup_groups_in_files(
                वर्ण * ध्रुव *files,
                ध्रुव वर्ण *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = रिक्त;
        ssize_t n_arr = 0;
        पूर्णाङ्क r;

        प्रतिज्ञा(files);
        प्रतिज्ञा(name);
        प्रतिज्ञा(gid_is_valid(gid));

        पूर्णाङ्क ngroups_max = sysconf_ngroups_max();
        यदि (ngroups_max < 0)
                प्रत्यागम ngroups_max;

        यदि (!GREEDY_REALLOC(arr, n_arr + 1))
                प्रत्यागम -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ खाता *f = रिक्त;
                संरचना group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                यदि (r == -ENOENT)
                        अनुवर्त;
                यदि (r < 0)
                        प्रत्यागम r;

                यावत् ((r = fgetgrent_sane(f, &gr)) > 0) {
                        यदि (gid == gr->gr_gid)
                                अनुवर्त;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        यदि (gid_list_has(arr, n_arr, gr->gr_gid))
                                अनुवर्त;

                        यदि (!strv_contains(gr->gr_mem, name))
                                अनुवर्त;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        यदि (n_arr >= ngroups_max)
                                प्रत्यागम -ENOBUFS;

                        यदि (!GREEDY_REALLOC(arr, n_arr + 1))
                                प्रत्यागम -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                यदि (r < 0)
                        प्रत्यागम r;
        }

        यदि (ret)
                *ret = TAKE_PTR(arr);
        प्रत्यागम n_arr;
}

पूर्णाङ्क getgrouplist_malloc(ध्रुव वर्ण *user, gid_t gid, gid_t **ret) {
#यदि BUILD_STATIC
        प्रत्यागम lookup_groups_in_files(GROUP_FILES, user, gid, ret);
#अन्यथा
        पूर्णाङ्क ngroups_max = sysconf_ngroups_max();
        यदि (ngroups_max < 0)
                प्रत्यागम ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        यदि (!gids)
                प्रत्यागम -ENOMEM;

        पूर्णाङ्क k = ngroups_max;
        यदि (getgrouplist(user, gid, gids, &k) < 0)
                प्रत्यागम -EIO;  /* getgrouplist does not set errno */

        यदि (ret)
                *ret = TAKE_PTR(gids);
        प्रत्यागम k;
#endif
}

पूर्णाङ्क initgroups_wrapper(ध्रुव वर्ण *user, gid_t gid) {
#यदि BUILD_STATIC
        _cleanup_free_ gid_t *groups = रिक्त;
        पूर्णाङ्क r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        यदि (n_groups <= 0)
                प्रत्यागम n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        प्रति (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                यदि (r != -EINVAL)
                        भङ्ग;
        }

        प्रत्यागम r;
#अन्यथा
        प्रत्यागम RET_NERRNO(initgroups(user, gid));
#endif
}

#यदि !BUILD_STATIC

स्थिर माप_प्रकार getgr_buffer_size(शून्य) {
        दीर्घ bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        प्रत्यागम bufsize <= 0 ? 4096U : (माप_प्रकार) bufsize;
}

#endif

पूर्णाङ्क getgrnam_malloc(ध्रुव वर्ण *name, संरचना group **ret) {
        यदि (isempty(name))
                प्रत्यागम -EINVAL;

#यदि BUILD_STATIC
        प्रत्यागम lookup_grent_in_files(GROUP_FILES, name, GID_INVALID, ret);
#अन्यथा
        माप_प्रकार bufsize = getgr_buffer_size();
        पूर्णाङ्क r;

        प्रति (;;) {
                _cleanup_free_ शून्य *buf = रिक्त;

                /* Silence static analyzers */
                प्रतिज्ञा(bufsize <= SIZE_MAX - ALIGN(परिमाण(संरचना group)));
                buf = malloc0(ALIGN(परिमाण(संरचना group)) + bufsize);
                यदि (!buf)
                        प्रत्यागम -ENOMEM;

                संरचना group *gr = रिक्त;
                r = getgrnam_r(name, buf, (वर्ण*) buf + ALIGN(परिमाण(संरचना group)), bufsize, &gr);
                यदि (r == 0) {
                        यदि (gr) {
                                यदि (ret)
                                        *ret = TAKE_PTR(buf);
                                प्रत्यागम 0;
                        }

                        प्रत्यागम -ESRCH;
                }

                प्रतिज्ञा(r > 0);

                यदि (errno_is_user_doesnt_exist(r))
                        प्रत्यागम -ESRCH;
                यदि (r != दुस्फल)
                        प्रत्यागम -r;

                यदि (bufsize > SIZE_MAX/2 - ALIGN(परिमाण(संरचना group)))
                        प्रत्यागम -ENOMEM;
                bufsize *= 2;
        }
#endif
}

पूर्णाङ्क getgrgid_malloc(gid_t gid, संरचना group **ret) {
        यदि (!gid_is_valid(gid))
                प्रत्यागम -EINVAL;

#यदि BUILD_STATIC
        प्रत्यागम lookup_grent_in_files(GROUP_FILES, /* name= */ रिक्त, gid, ret);
#अन्यथा
        माप_प्रकार bufsize = getgr_buffer_size();
        पूर्णाङ्क r;

        प्रति (;;) {
                _cleanup_free_ शून्य *buf = रिक्त;

                /* Silence static analyzers */
                प्रतिज्ञा(bufsize <= SIZE_MAX - ALIGN(परिमाण(संरचना group)));
                buf = malloc0(ALIGN(परिमाण(संरचना group)) + bufsize);
                यदि (!buf)
                        प्रत्यागम -ENOMEM;

                संरचना group *gr = रिक्त;
                r = getgrgid_r(gid, buf, (वर्ण*) buf + ALIGN(परिमाण(संरचना group)), bufsize, &gr);
                यदि (r == 0) {
                        यदि (gr) {
                                यदि (ret)
                                        *ret = TAKE_PTR(buf);
                                प्रत्यागम 0;
                        }

                        प्रत्यागम -ESRCH;
                }

                प्रतिज्ञा(r > 0);

                यदि (errno_is_user_doesnt_exist(r))
                        प्रत्यागम -ESRCH;
                यदि (r != दुस्फल)
                        प्रत्यागम -r;

                यदि (bufsize > SIZE_MAX/2 - ALIGN(परिमाण(संरचना group)))
                        प्रत्यागम -ENOMEM;
                bufsize *= 2;
        }
#endif
}
