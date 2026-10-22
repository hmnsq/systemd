/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <sys/file.h>
#అన్తర్భావ <sys/stat.h>
#అన్తర్భావ <unistd.h>
#అన్తర్భావ <utmpx.h>

#అన్తర్భావ "sd-messages.h"

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "chase.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "extract-word.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "format-util.h"
#అన్తర్భావ "lock-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "mkdir.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "terminal-util.h"
#అన్తర్భావ "user-util.h"
#అన్తర్భావ "utf8.h"

#నిర్వచించు DEFINE_STRERROR_ACCOUNT(type)                                   \
        స్థిరరాశి అక్షరం* strerror_##type(                                    \
                        పూర్ణాంకం errnum,                                     \
                        అక్షరం *buf,                                      \
                        size_t buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                ఎంపిక (errnum) {                                       \
                స్థితి ESRCH:                                             \
                        తిరిగి "Unknown " STRINGIFY(type);              \
                స్థితి ENOEXEC:                                           \
                        తిరిగి "Not a system " STRINGIFY(type);         \
                అప్రమేయ:                                                \
                        తిరిగి strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        ఒకవేళ (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                తిరిగి false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        ఒకవేళ (uid == (uid_t) UINT32_C(0xFFFF))
                తిరిగి false;

        తిరిగి true;
}

పూర్ణాంకం parse_uid(స్థిరరాశి అక్షరం *s, uid_t *ret) {
        uint32_t uid = 0;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(s);

        assert_cc(పరిమాణం(uid_t) == పరిమాణం(uint32_t));

        /* We are very strict when parsing UIDs, and prohibit +/- as prefix, leading zero as prefix, and
         * whitespace. We do this, since this call is often used in a context where we parse things as UID
         * first, and if that doesn't work we fall back to NSS. Thus we really want to make sure that UIDs
         * are parsed as UIDs only if they really really look like UIDs. */
        r = సురక్షిత_atou32_నిండిన(s, 10
                             | సురక్షిత_ATO_REFUSE_PLUS_MINUS
                             | సురక్షిత_ATO_REFUSE_LEADING_ZERO
                             | సురక్షిత_ATO_REFUSE_LEADING_WHITESPACE, &uid);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (!uid_is_valid(uid))
                తిరిగి -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        ఒకవేళ (ret)
                *ret = uid;

        తిరిగి 0;
}

పూర్ణాంకం parse_uid_range(స్థిరరాశి అక్షరం *s, uid_t *ret_క్రింది, uid_t *ret_పై) {
        _cleanup_free_ అక్షరం *మాట = ఖాళీ;
        uid_t l, u;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(ret_క్రింది);
        ప్రతిజ్ఞా(ret_పై);

        r = వెలికితీయి_మొదటి_మాట(&s, &మాట, "-", వెలికితీయి_DONT_COALESCE_SEPARATORS);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == 0)
                తిరిగి -EINVAL;

        r = parse_uid(మాట, &l);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* Check for the upper bound and extract it if needed */
        ఒకవేళ (!s)
                /* Single number with no dash. */
                u = l;
        లేకపోతే ఒకవేళ (!*s)
                /* Trailing dash is an error. */
                తిరిగి -EINVAL;
        లేకపోతే {
                r = parse_uid(s, &u);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (l > u)
                        తిరిగి -EINVAL;
        }

        *ret_క్రింది = l;
        *ret_పై = u;
        తిరిగి 0;
}

అక్షరం* getlogname_malloc(శూన్య) {
        uid_t uid;
        నిర్మాణం stat st;

        ఒకవేళ (isatty_సురక్షిత(STDIN_FILENO) && fstat(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        లేకపోతే
                uid = getuid();

        తిరిగి uid_గమ్యం_name(uid);
}

అక్షరం* getusername_malloc(శూన్య) {
        స్థిరరాశి అక్షరం *e;

        e = secure_getenv("USER");
        ఒకవేళ (e)
                తిరిగి strdup(e);

        తిరిగి uid_గమ్యం_name(getuid());
}

bool is_nologin_shell(స్థిరరాశి అక్షరం *shell) {
        తిరిగి మార్గం_లోపల_సమితి(shell,
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

bool shell_is_placeholder(స్థిరరాశి అక్షరం *shell) {
        తిరిగి isempty(shell) || is_nologin_shell(shell);
}

స్థిరరాశి అక్షరం* default_మూలం_shell_at(పూర్ణాంకం rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        ప్రతిజ్ఞా(rfd >= 0 || rfd == AT_FDCWD);

        పూర్ణాంకం r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, ఖాళీ, ఖాళీ);
        ఒకవేళ (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        ఒకవేళ (r > 0)
                తిరిగి DEFAULT_USER_SHELL;

        తిరిగి "/bin/sh";
}

స్థిరరాశి అక్షరం* default_మూలం_shell(స్థిరరాశి అక్షరం *మూలం) {
        _cleanup_close_ పూర్ణాంకం rfd = -EBADF;

        rfd = open(ఖాళీలు_గమ్యం_మూలం(మూలం), O_CLOEXEC | O_DIRECTORY | O_మార్గం);
        ఒకవేళ (rfd < 0)
                తిరిగి "/bin/sh";

        తిరిగి default_మూలం_shell_at(rfd);
}

స్థిర పూర్ణాంకం return_user_creds(
                స్థిరరాశి అక్షరం *username,
                uid_t uid, gid_t gid,
                స్థిరరాశి అక్షరం *home,
                స్థిరరాశి అక్షరం *shell,
                అక్షరం **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                అక్షరం **ret_home,
                అక్షరం **ret_shell) {
        /* Helper function to help with the strdups and atomic setting of return params. */

        _cleanup_free_ అక్షరం *s1 = ఖాళీ, *s2 = ఖాళీ, *s3 = ఖాళీ;
        పూర్ణాంకం r;

        ఒకవేళ (ret_username) {
                r = strdup_గమ్యం(&s1, username);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        ఒకవేళ (ret_home) {
                r = strdup_గమ్యం(&s2, home);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        ఒకవేళ (ret_shell) {
                r = strdup_గమ్యం(&s3, shell);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        ఒకవేళ (ret_username)
                *ret_username = TAKE_PTR(s1);
        ఒకవేళ (ret_uid)
                *ret_uid = uid;
        ఒకవేళ (ret_gid)
                *ret_gid = gid;
        ఒకవేళ (ret_home)
                *ret_home = TAKE_PTR(s2);
        ఒకవేళ (ret_shell)
                *ret_shell = TAKE_PTR(s3);
        తిరిగి 0;
}

స్థిర పూర్ణాంకం synthesize_user_creds(
                స్థిరరాశి అక్షరం *username,
                UserCredsFlags flags,
                అక్షరం **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                అక్షరం **ret_home,
                అక్షరం **ret_shell) {
        ప్రతిజ్ఞా(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        ఒకవేళ (STR_లోపల_సమితి(username, "root", "0"))
                తిరిగి return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_మూలం_shell(ఖాళీ) : ఖాళీ,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        ఒకవేళ (STR_లోపల_సమితి(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                తిరిగి return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_సమితి(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? ఖాళీ : "/",
                                         FLAGS_సమితి(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? ఖాళీ : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        తిరిగి -ENOMEDIUM;
}

పూర్ణాంకం get_user_creds(
                స్థిరరాశి అక్షరం *username,
                UserCredsFlags flags,
                అక్షరం **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                అక్షరం **ret_home,
                అక్షరం **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ నిర్మాణం passwd *pw = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(username);
        ప్రతిజ్ఞా((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        ఒకవేళ (!FLAGS_సమితి(flags, USER_CREDS_PREFER_NSS) ||
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
                ఒకవేళ (r >= 0)
                        తిరిగి 0;
                ఒకవేళ (r != -ENOMEDIUM) /* not a username we can synthesize */
                        తిరిగి r;
        }

        ఒకవేళ (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                ఒకవేళ (r >= 0)
                        username = pw->pw_name;

                లేకపోతే ఒకవేళ (FLAGS_సమితి(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        ఒకవేళ (ret_username)
                                *ret_username = ఖాళీ;
                        ఒకవేళ (ret_uid)
                                *ret_uid = u;
                        తిరిగి 0;
                }
        } లేకపోతే
                r = getpwnam_malloc(username, &pw);

        ఒకవేళ (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                ఒకవేళ (FLAGS_సమితి(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        తిరిగి 0;

                తిరిగి r;
        }

        ఒకవేళ (ret_uid && !uid_is_valid(pw->pw_uid))
                తిరిగి -EBADMSG;

        ఒకవేళ (ret_gid && !gid_is_valid(pw->pw_gid))
                తిరిగి -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        స్థిరరాశి అక్షరం *h =
                (FLAGS_సమితి(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && ఖాళీలు_or_మూలం(pw->pw_dir)) ||
                (FLAGS_సమితి(flags, USER_CREDS_CLEAN) && (!మార్గం_is_valid(pw->pw_dir) || !మార్గం_is_absolute(pw->pw_dir)))
                ? ఖాళీ : pw->pw_dir;

        స్థిరరాశి అక్షరం *s =
                (FLAGS_సమితి(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_సమితి(flags, USER_CREDS_CLEAN) && (!మార్గం_is_valid(pw->pw_shell) || !మార్గం_is_absolute(pw->pw_shell)))
                ? ఖాళీ : pw->pw_shell;

        తిరిగి return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

స్థిర పూర్ణాంకం synthesize_సమూహం_creds(స్థిరరాశి అక్షరం *groupname, అక్షరం **ret_name, gid_t *ret_gid) {
        ప్రతిజ్ఞా(groupname);

        gid_t id;
        స్థిరరాశి అక్షరం *n;
        పూర్ణాంకం r;

        ఒకవేళ (STR_లోపల_సమితి(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } లేకపోతే ఒకవేళ (STR_లోపల_సమితి(groupname, NOBODY_సమూహం_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_సమూహం_NAME;
        } లేకపోతే
                తిరిగి -ENOMEDIUM;

        r = strdup_గమ్యం_నిండిన(ret_name, n);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (ret_gid)
                *ret_gid = id;
        తిరిగి 0;
}

పూర్ణాంకం get_సమూహం_creds(స్థిరరాశి అక్షరం *groupname, UserCredsFlags flags, అక్షరం **ret_name, gid_t *ret_gid) {
        _cleanup_free_ నిర్మాణం group *gr = ఖాళీ;
        gid_t id;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(groupname);

        ఒకవేళ (!FLAGS_సమితి(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_సమూహం_creds(groupname, ret_name, ret_gid);
                ఒకవేళ (r >= 0)
                        తిరిగి 0;
                ఒకవేళ (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        తిరిగి r;
        }

        ఒకవేళ (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                ఒకవేళ (r >= 0)
                        groupname = gr->gr_name;
                లేకపోతే ఒకవేళ (FLAGS_సమితి(flags, USER_CREDS_ALLOW_MISSING)) {
                        ఒకవేళ (ret_gid)
                                *ret_gid = id;
                        ఒకవేళ (ret_name)
                                *ret_name = ఖాళీ;
                        తిరిగి 0;
                }
        } లేకపోతే
                r = getgrnam_malloc(groupname, &gr);

        ఒకవేళ (r < 0) {
                ఒకవేళ (FLAGS_సమితి(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_సమూహం_creds(groupname, ret_name, ret_gid) >= 0)
                        తిరిగి 0;
                తిరిగి r;
        }

        ఒకవేళ (ret_gid && !gid_is_valid(gr->gr_gid))
                తిరిగి -EBADMSG;

        r = strdup_గమ్యం_నిండిన(ret_name, groupname);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (ret_gid)
                *ret_gid = gr->gr_gid;
        తిరిగి 0;
}

అక్షరం* uid_గమ్యం_name(uid_t uid) {
        అక్షరం *ret;
        పూర్ణాంకం r;

        /* Shortcut things to avoid NSS lookups */
        ఒకవేళ (uid == 0)
                తిరిగి strdup("root");
        ఒకవేళ (uid == UID_NOBODY && synthesize_nobody())
                తిరిగి strdup(NOBODY_USER_NAME);

        ఒకవేళ (uid_is_valid(uid)) {
                _cleanup_free_ నిర్మాణం passwd *pw = ఖాళీ;

                r = getpwuid_malloc(uid, &pw);
                ఒకవేళ (r >= 0)
                        తిరిగి strdup(pw->pw_name);
        }

        ఒకవేళ (asprintf(&ret, UID_FMT, uid) < 0)
                తిరిగి ఖాళీ;

        తిరిగి ret;
}

అక్షరం* gid_గమ్యం_name(gid_t gid) {
        అక్షరం *ret;
        పూర్ణాంకం r;

        ఒకవేళ (gid == 0)
                తిరిగి strdup("root");
        ఒకవేళ (gid == GID_NOBODY && synthesize_nobody())
                తిరిగి strdup(NOBODY_సమూహం_NAME);

        ఒకవేళ (gid_is_valid(gid)) {
                _cleanup_free_ నిర్మాణం group *gr = ఖాళీ;

                r = getgrgid_malloc(gid, &gr);
                ఒకవేళ (r >= 0)
                        తిరిగి strdup(gr->gr_name);
        }

        ఒకవేళ (asprintf(&ret, GID_FMT, gid) < 0)
                తిరిగి ఖాళీ;

        తిరిగి ret;
}

స్థిర bool gid_list_has(స్థిరరాశి gid_t *list, size_t కొలత, gid_t val) {
        ప్రతిజ్ఞా(list || కొలత == 0);

        FOREACH_ARRAY(i, list, కొలత)
                ఒకవేళ (*i == val)
                        తిరిగి true;

        తిరిగి false;
}

పూర్ణాంకం లోపల_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = ఖాళీ;
        పూర్ణాంకం ngroups;

        ఒకవేళ (getgid() == gid)
                తిరిగి 1;

        ఒకవేళ (getegid() == gid)
                తిరిగి 1;

        ఒకవేళ (!gid_is_valid(gid))
                తిరిగి -EINVAL;

        ngroups = getgroups_alloc(&gids);
        ఒకవేళ (ngroups < 0)
                తిరిగి ngroups;

        తిరిగి gid_list_has(gids, ngroups, gid);
}

పూర్ణాంకం లోపల_సమూహం(స్థిరరాశి అక్షరం *name) {
        పూర్ణాంకం r;
        gid_t gid;

        r = get_సమూహం_creds(name, /* flags= */ 0, /* ret_name= */ ఖాళీ, &gid);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి లోపల_gid(gid);
}

పూర్ణాంకం విలీనం_gid_lists(స్థిరరాశి gid_t *list1, size_t size1, స్థిరరాశి gid_t *list2, size_t size2, gid_t **ret) {
        size_t nresult = 0;

        ప్రతిజ్ఞా(size1 == 0 || list1);
        ప్రతిజ్ఞా(size2 == 0 || list2);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (size2 > గరిష్ఠ_పూర్ణాంకం - size1)
                తిరిగి -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        ఒకవేళ (!buf)
                తిరిగి -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        కొరకు (size_t i = 0; i < size1; i++)
                ఒకవేళ (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        కొరకు (size_t i = 0; i < size2; i++)
                ఒకవేళ (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        తిరిగి (పూర్ణాంకం)nresult;
}

పూర్ణాంకం getgroups_alloc(gid_t **ret) {
        పూర్ణాంకం ngroups = 8;

        ప్రతిజ్ఞా(ret);

        కొరకు (గుర్తులేని attempt = 0;;) {
                _cleanup_free_ gid_t *p = ఖాళీ;

                p = new(gid_t, ngroups);
                ఒకవేళ (!p)
                        తిరిగి -ENOMEM;

                ngroups = getgroups(ngroups, p);
                ఒకవేళ (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        తిరిగి ngroups;
                }
                ఒకవేళ (ngroups == 0)
                        ఆపు;
                ఒకవేళ (errno != EINVAL)
                        తిరిగి -errno;

                /* Give up eventually */
                ఒకవేళ (attempt++ > 10)
                        తిరిగి -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, ఖాళీ);
                ఒకవేళ (ngroups < 0)
                        తిరిగి -errno;
                ఒకవేళ (ngroups == 0)
                        ఆపు;
        }

        *ret = ఖాళీ;
        తిరిగి 0;
}

పూర్ణాంకం get_home_dir(అక్షరం **ret) {
        _cleanup_free_ నిర్మాణం passwd *p = ఖాళీ;
        స్థిరరాశి అక్షరం *e;
        uid_t u;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        ఒకవేళ (e && మార్గం_is_valid(e) && మార్గం_is_absolute(e))
                వెళ్ళు దొరికింది;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        ఒకవేళ (u == 0) {
                e = "/root";
                వెళ్ళు దొరికింది;
        }
        ఒకవేళ (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                వెళ్ళు దొరికింది;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        ఒకవేళ (r < 0)
                తిరిగి r;

        e = p->pw_dir;
        ఒకవేళ (!మార్గం_is_valid(e) || !మార్గం_is_absolute(e))
                తిరిగి -EINVAL;

 దొరికింది:
        తిరిగి మార్గం_simplify_alloc(e, ret);
}

పూర్ణాంకం get_shell(అక్షరం **ret) {
        _cleanup_free_ నిర్మాణం passwd *p = ఖాళీ;
        స్థిరరాశి అక్షరం *e;
        uid_t u;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        ఒకవేళ (e && మార్గం_is_valid(e) && మార్గం_is_absolute(e))
                వెళ్ళు దొరికింది;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        ఒకవేళ (u == 0) {
                e = default_మూలం_shell(ఖాళీ);
                వెళ్ళు దొరికింది;
        }
        ఒకవేళ (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                వెళ్ళు దొరికింది;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        ఒకవేళ (r < 0)
                తిరిగి r;

        e = p->pw_shell;
        ఒకవేళ (!మార్గం_is_valid(e) || !మార్గం_is_absolute(e))
                తిరిగి -EINVAL;

 దొరికింది:
        తిరిగి మార్గం_simplify_alloc(e, ret);
}

పూర్ణాంకం fully_సమితి_uid_gid(uid_t uid, gid_t gid, స్థిరరాశి gid_t supplementary_gids[], size_t n_supplementary_gids) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (gid_is_valid(gid))
                ఒకవేళ (setresgid(gid, gid, gid) < 0)
                        తిరిగి -errno;

        ఒకవేళ (uid_is_valid(uid))
                ఒకవేళ (setresuid(uid, uid, uid) < 0)
                        తిరిగి -errno;

        తిరిగి 0;
}

పూర్ణాంకం take_etc_passwd_తాళం(స్థిరరాశి అక్షరం *మూలం) {
        పూర్ణాంకం r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ అక్షరం *మార్గం = మార్గం_join(మూలం, ETC_PASSWD_తాళం_మార్గం);
        ఒకవేళ (!మార్గం)
                తిరిగి log_oom_debug();

        (శూన్య) mkdir_parents(మార్గం, 0755);

        _cleanup_close_ పూర్ణాంకం fd = open(మార్గం, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        ఒకవేళ (fd < 0)
                తిరిగి log_debug_errno(errno, "Cannot open %s: %m", మార్గం);

        r = unposix_తాళం(fd, LOCK_EX);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Locking %s failed: %m", మార్గం);

        తిరిగి TAKE_FD(fd);
}

bool valid_user_సమూహం_name(స్థిరరాశి అక్షరం *u, ValidUserFlags flags) {
        స్థిరరాశి అక్షరం *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        ఒకవేళ (isempty(u)) /* An empty user name is never valid */
                తిరిగి false;

        ఒకవేళ (parse_uid(u, ఖాళీ) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                తిరిగి FLAGS_సమితి(flags, VALID_USER_ALLOW_NUMERIC);

        ఒకవేళ (FLAGS_సమితి(flags, VALID_USER_RELAX)) {

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

                ఒకవేళ (startswith(u, " ") || endswith(u, " ")) /* At least expect whitespace padding is removed
                                                             * at front and back (accept in the middle, since
                                                             * that's apparently a thing on Windows). Note
                                                             * that this also blocks usernames consisting of
                                                             * whitespace only. */
                        తిరిగి false;

                ఒకవేళ (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        తిరిగి false;

                ఒకవేళ (string_has_cc(u, ఖాళీ)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        తిరిగి false;

                ఒకవేళ (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        తిరిగి false;

                ఒకవేళ (లోపల_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        తిరిగి false;

                ఒకవేళ (u[0] == '-' && లోపల_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        తిరిగి false;

                ఒకవేళ (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        తిరిగి false;

                /* Compare with strict result and warn if result doesn't match */
                ఒకవేళ (FLAGS_సమితి(flags, VALID_USER_WARN) && !valid_user_సమూహం_name(u, 0))
                        log_struct(LOG_NOTICE,
                                   LOG_సందేశం("Accepting user/group name '%s', which does not match strict user/group name rules.", u),
                                   LOG_అంశం("USER_GROUP_NAME=%s", u),
                                   LOG_సందేశం_ID(SD_సందేశం_UNSAFE_USER_NAME_STR));

                /* Note that we make no restrictions on the length in relaxed mode! */
        } లేకపోతే {
                దీర్ఘ sz;
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

                ఒకవేళ (!ascii_isalpha(u[0]) &&
                    u[0] != '_')
                        తిరిగి false;

                కొరకు (i = u+1; *i; i++)
                        ఒకవేళ (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !లోపల_సమితి(*i, '_', '-'))
                                తిరిగి false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                ఒకవేళ (l > (size_t) sz) /* glibc: 256 */
                        తిరిగి false;
                ఒకవేళ (l > NAME_MAX) /* must fit in a filename: 255 */
                        తిరిగి false;
                ఒకవేళ (l > sizeof_క్షేత్రం(నిర్మాణం utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        తిరిగి false;
        }

        తిరిగి true;
}

bool valid_gecos(స్థిరరాశి అక్షరం *d) {

        ఒకవేళ (!d)
                తిరిగి false;

        ఒకవేళ (!utf8_is_valid(d))
                తిరిగి false;

        ఒకవేళ (string_has_cc(d, ఖాళీ))
                తిరిగి false;

        /* Colons are used as field separators, and hence not OK */
        ఒకవేళ (strchr(d, ':'))
                తిరిగి false;

        తిరిగి true;
}

అక్షరం* mangle_gecos(స్థిరరాశి అక్షరం *d) {
        అక్షరం *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        ఒకవేళ (!mangled)
                తిరిగి ఖాళీ;

        కొరకు (అక్షరం *i = mangled; *i; i++) {
                పూర్ణాంకం len;

                ఒకవేళ ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        కొనసాగు;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                ఒకవేళ (len < 0) {
                        *i = ' ';
                        కొనసాగు;
                }

                i += len - 1;
        }

        తిరిగి mangled;
}

bool valid_home(స్థిరరాశి అక్షరం *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        ఒకవేళ (isempty(p))
                తిరిగి false;

        ఒకవేళ (!utf8_is_valid(p))
                తిరిగి false;

        ఒకవేళ (string_has_cc(p, ఖాళీ))
                తిరిగి false;

        ఒకవేళ (!మార్గం_is_absolute(p))
                తిరిగి false;

        ఒకవేళ (!మార్గం_is_normalized(p))
                తిరిగి false;

        /* Colons are used as field separators, and hence not OK */
        ఒకవేళ (strchr(p, ':'))
                తిరిగి false;

        తిరిగి true;
}

bool valid_shell(స్థిరరాశి అక్షరం *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        ఒకవేళ (!valid_home(p))
                తిరిగి false;

        తిరిగి !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

పూర్ణాంకం maybe_setgroups(size_t కొలత, స్థిరరాశి gid_t *list) {
        పూర్ణాంకం r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        ఒకవేళ (కొలత == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ FILE *f = fopen("/proc/self/gid_map", "re");
                ఒకవేళ (!f && errno != ENOENT)
                        తిరిగి -errno;
                ఒకవేళ (f) {
                        r = సురక్షిత_fgetc(f, /* ret= */ ఖాళీ);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                        ఒకవేళ (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                తిరిగి 0;
                        }
                }

                _cleanup_free_ అక్షరం *setgroups_content = ఖాళీ;
                r = read_one_పంక్తి_file("/proc/self/setgroups", &setgroups_content);
                ఒకవేళ (r < 0 && r != -ENOENT)
                        తిరిగి r;
                ఒకవేళ (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        తిరిగి 0;
                }
        }

        తిరిగి RET_NERRNO(setgroups(కొలత, list));
}

bool synthesize_nobody(శూన్య) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        స్థిర పూర్ణాంకం cache = -1;

        ఒకవేళ (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        తిరిగి cache;
}

పూర్ణాంకం putpwent_sane(స్థిరరాశి నిర్మాణం passwd *pw, FILE *ప్రవాహం) {
        ప్రతిజ్ఞా(pw);
        ప్రతిజ్ఞా(ప్రవాహం);

        errno = 0;
        ఒకవేళ (putpwent(pw, ప్రవాహం) != 0)
                తిరిగి errno_or_else(EIO);

        తిరిగి 0;
}

పూర్ణాంకం putspent_sane(స్థిరరాశి నిర్మాణం spwd *sp, FILE *ప్రవాహం) {
        ప్రతిజ్ఞా(sp);
        ప్రతిజ్ఞా(ప్రవాహం);

        errno = 0;
        ఒకవేళ (putspent(sp, ప్రవాహం) != 0)
                తిరిగి errno_or_else(EIO);

        తిరిగి 0;
}

పూర్ణాంకం putgrent_sane(స్థిరరాశి నిర్మాణం group *gr, FILE *ప్రవాహం) {
        ప్రతిజ్ఞా(gr);
        ప్రతిజ్ఞా(ప్రవాహం);

        errno = 0;
        ఒకవేళ (putgrent(gr, ప్రవాహం) != 0)
                తిరిగి errno_or_else(EIO);

        తిరిగి 0;
}

#ఒకవేళ ENABLE_GSHADOW
పూర్ణాంకం putsgent_sane(స్థిరరాశి నిర్మాణం sgrp *sg, FILE *ప్రవాహం) {
        ప్రతిజ్ఞా(sg);
        ప్రతిజ్ఞా(ప్రవాహం);

        errno = 0;
        ఒకవేళ (putsgent(sg, ప్రవాహం) != 0)
                తిరిగి errno_or_else(EIO);

        తిరిగి 0;
}
#అయితే_ముగింపు

పూర్ణాంకం fgetpwent_sane(FILE *ప్రవాహం, నిర్మాణం passwd **pw) {
        ప్రతిజ్ఞా(ప్రవాహం);
        ప్రతిజ్ఞా(pw);

        errno = 0;
        నిర్మాణం passwd *p = fgetpwent(ప్రవాహం);
        ఒకవేళ (!p && !లోపల_సమితి(errno, 0, ENOENT))
                తిరిగి -errno;

        *pw = p;
        తిరిగి !!p;
}

పూర్ణాంకం fgetspent_sane(FILE *ప్రవాహం, నిర్మాణం spwd **sp) {
        ప్రతిజ్ఞా(ప్రవాహం);
        ప్రతిజ్ఞా(sp);

        errno = 0;
        నిర్మాణం spwd *s = fgetspent(ప్రవాహం);
        ఒకవేళ (!s && !లోపల_సమితి(errno, 0, ENOENT))
                తిరిగి -errno;

        *sp = s;
        తిరిగి !!s;
}

పూర్ణాంకం fgetgrent_sane(FILE *ప్రవాహం, నిర్మాణం group **gr) {
        ప్రతిజ్ఞా(ప్రవాహం);
        ప్రతిజ్ఞా(gr);

        errno = 0;
        నిర్మాణం group *g = fgetgrent(ప్రవాహం);
        ఒకవేళ (!g && !లోపల_సమితి(errno, 0, ENOENT))
                తిరిగి -errno;

        *gr = g;
        తిరిగి !!g;
}

#ఒకవేళ ENABLE_GSHADOW
పూర్ణాంకం fgetsgent_sane(FILE *ప్రవాహం, నిర్మాణం sgrp **sg) {
        ప్రతిజ్ఞా(ప్రవాహం);
        ప్రతిజ్ఞా(sg);

        errno = 0;
        నిర్మాణం sgrp *s = fgetsgent(ప్రవాహం);
        ఒకవేళ (!s && !లోపల_సమితి(errno, 0, ENOENT))
                తిరిగి -errno;

        *sg = s;
        తిరిగి !!s;
}
#అయితే_ముగింపు

పూర్ణాంకం is_this_me(స్థిరరాశి అక్షరం *username) {
        uid_t uid;
        పూర్ణాంకం r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, ఖాళీ, &uid, ఖాళీ, ఖాళీ, ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి uid == getuid();
}

స్థిరరాశి అక్షరం* get_home_మూలం(శూన్య) {
        /* For debug purposes allow overriding where we look for home dirs */
        స్థిరరాశి అక్షరం *e = secure_getenv("SYSTEMD_HOME_ROOT");
        ఒకవేళ (e && మార్గం_is_absolute(e) && మార్గం_is_normalized(e))
                తిరిగి e;

        తిరిగి "/home";
}

స్థిర పూర్ణాంకం copy_struct_passwd(స్థిరరాశి నిర్మాణం passwd *pw, నిర్మాణం passwd **ret) {
        ప్రతిజ్ఞా(pw);
        ప్రతిజ్ఞా(ret);

        size_t need_బైట్లు = పరిమాణం(నిర్మాణం passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        అక్షరం *buf = జ్ఞాపకం_ఇవ్వు(need_బైట్లు);
        ఒకవేళ (!buf)
                తిరిగి -ENOMEM;

        నిర్మాణం passwd *newpw = (నిర్మాణం passwd *) buf;

        /* The layout in our buffer:
         * struct passwd, and then individual strings. */
        అక్షరం *p = buf + పరిమాణం(నిర్మాణం passwd);

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
        తిరిగి 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
పూర్ణాంకం lookup_pwent_లోపల_files(
                అక్షరం * స్థిరరాశి *files,
                స్థిరరాశి అక్షరం *name,
                uid_t uid,
                నిర్మాణం passwd **ret) {

        పూర్ణాంకం r;

        ప్రతిజ్ఞా(files);
        ప్రతిజ్ఞా(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ఖాళీ;
                నిర్మాణం passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                ఒకవేళ (r == -ENOENT)
                        కొనసాగు;
                ఒకవేళ (r < 0)
                        తిరిగి r;

                అయితే ((r = fgetpwent_sane(f, &pw)) > 0) {
                        ఒకవేళ (name && !streq_ptr(pw->pw_name, name))
                                కొనసాగు;
                        ఒకవేళ (uid_is_valid(uid) && pw->pw_uid != uid)
                                కొనసాగు;
                        ఒకవేళ (ret)
                                తిరిగి copy_struct_passwd(pw, ret);
                        తిరిగి 0;
                }
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        తిరిగి -ESRCH;
}

#ఒకవేళ !BUILD_STATIC

స్థిర size_t getpw_బఫరు_కొలత(శూన్య) {
        దీర్ఘ bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        తిరిగి bufsize <= 0 ? 4096U : (size_t) bufsize;
}

స్థిర bool errno_is_user_doesnt_exist(పూర్ణాంకం error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        తిరిగి లోపల_సమితి(నిరపేక్ష(error), ENOENT, ESRCH, EBADF, EPERM);
}

#అయితే_ముగింపు

పూర్ణాంకం getpwnam_malloc(స్థిరరాశి అక్షరం *name, నిర్మాణం passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        ఒకవేళ (isempty(name))
                తిరిగి -EINVAL;

#ఒకవేళ BUILD_STATIC
        తిరిగి lookup_pwent_లోపల_files(PASSWD_FILES, name, UID_INVALID, ret);
#లేకపోతే
        size_t bufsize = getpw_బఫరు_కొలత();
        పూర్ణాంకం r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        కొరకు (;;) {
                _cleanup_free_ శూన్య *buf = ఖాళీ;

                /* Silence static analyzers */
                ప్రతిజ్ఞా(bufsize <= SIZE_MAX - ALIGN(పరిమాణం(నిర్మాణం passwd)));
                buf = malloc0(ALIGN(పరిమాణం(నిర్మాణం passwd)) + bufsize);
                ఒకవేళ (!buf)
                        తిరిగి -ENOMEM;

                నిర్మాణం passwd *pw = ఖాళీ;
                r = getpwnam_r(name, buf, (అక్షరం*) buf + ALIGN(పరిమాణం(నిర్మాణం passwd)), bufsize, &pw);
                ఒకవేళ (r == 0) {
                        ఒకవేళ (pw) {
                                ఒకవేళ (ret)
                                        *ret = TAKE_PTR(buf);
                                తిరిగి 0;
                        }

                        తిరిగి -ESRCH;
                }

                ప్రతిజ్ఞా(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                ఒకవేళ (errno_is_user_doesnt_exist(r))
                        తిరిగి -ESRCH;
                ఒకవేళ (r != ERANGE)
                        తిరిగి -r;

                ఒకవేళ (bufsize > SIZE_MAX/2 - ALIGN(పరిమాణం(నిర్మాణం passwd)))
                        తిరిగి -ENOMEM;
                bufsize *= 2;
        }
#అయితే_ముగింపు
}

పూర్ణాంకం getpwuid_malloc(uid_t uid, నిర్మాణం passwd **ret) {
        ఒకవేళ (!uid_is_valid(uid))
                తిరిగి -EINVAL;

#ఒకవేళ BUILD_STATIC
        తిరిగి lookup_pwent_లోపల_files(PASSWD_FILES, /* name= */ ఖాళీ, uid, ret);
#లేకపోతే
        size_t bufsize = getpw_బఫరు_కొలత();
        పూర్ణాంకం r;

        కొరకు (;;) {
                _cleanup_free_ శూన్య *buf = ఖాళీ;

                /* Silence static analyzers */
                ప్రతిజ్ఞా(bufsize <= SIZE_MAX - ALIGN(పరిమాణం(నిర్మాణం passwd)));
                buf = malloc0(ALIGN(పరిమాణం(నిర్మాణం passwd)) + bufsize);
                ఒకవేళ (!buf)
                        తిరిగి -ENOMEM;

                నిర్మాణం passwd *pw = ఖాళీ;
                r = getpwuid_r(uid, buf, (అక్షరం*) buf + ALIGN(పరిమాణం(నిర్మాణం passwd)), bufsize, &pw);
                ఒకవేళ (r == 0) {
                        ఒకవేళ (pw) {
                                ఒకవేళ (ret)
                                        *ret = TAKE_PTR(buf);
                                తిరిగి 0;
                        }

                        తిరిగి -ESRCH;
                }

                ప్రతిజ్ఞా(r > 0);

                ఒకవేళ (errno_is_user_doesnt_exist(r))
                        తిరిగి -ESRCH;
                ఒకవేళ (r != ERANGE)
                        తిరిగి -r;

                ఒకవేళ (bufsize > SIZE_MAX/2 - ALIGN(పరిమాణం(నిర్మాణం passwd)))
                        తిరిగి -ENOMEM;
                bufsize *= 2;
        }
#అయితే_ముగింపు
}

స్థిర పూర్ణాంకం copy_struct_సమూహం(స్థిరరాశి నిర్మాణం group *gr, నిర్మాణం group **ret) {
        ప్రతిజ్ఞా(gr);
        ప్రతిజ్ఞా(ret);

        size_t need_బైట్లు = పరిమాణం(నిర్మాణం group)
                + strlen_ptr(gr->gr_name) + 1
                + strlen_ptr(gr->gr_passwd) + 1,
                n_mem = 0;
        STRV_FOREACH(s, gr->gr_mem) {
                need_బైట్లు += పరిమాణం(అక్షరం*) + అక్షరశ్రేణి_పొడవు(*s) + 1;
                n_mem++;
        }
        need_బైట్లు += పరిమాణం(అక్షరం*);  /* NULL terminator for gr_mem */

        అక్షరం *buf = జ్ఞాపకం_ఇవ్వు(need_బైట్లు);
        ఒకవేళ (!buf)
                తిరిగి -ENOMEM;

        నిర్మాణం group *newgr = (నిర్మాణం group *) buf;

        /* The layout in our buffer:
         * struct group, ->gr_mem pointers terminated by NULL, ->gr_name, ->gr_passwd, ->gr_mem items */
        /* The ->gr_mem array is first, because it needs alignment. */
        assert_cc(పరిమాణం(నిర్మాణం group) % alignof(అక్షరం*) == 0);

        అక్షరం *p = buf + పరిమాణం(నిర్మాణం group) + (n_mem + 1) * పరిమాణం(అక్షరం*);

        newgr->gr_name = p;
        p = stpcpy(p, strempty(gr->gr_name)) + 1;

        newgr->gr_passwd = p;
        p = stpcpy(p, strempty(gr->gr_passwd)) + 1;

        newgr->gr_gid = gr->gr_gid;

        newgr->gr_mem = (అక్షరం**) (buf + పరిమాణం(నిర్మాణం group));
        కొరకు (size_t i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = ఖాళీ;

        *ret = newgr;
        తిరిగి 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
పూర్ణాంకం lookup_grent_లోపల_files(
                అక్షరం * స్థిరరాశి *files,
                స్థిరరాశి అక్షరం *name,
                gid_t gid,
                నిర్మాణం group **ret) {

        పూర్ణాంకం r;

        ప్రతిజ్ఞా(files);
        ప్రతిజ్ఞా(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ఖాళీ;
                నిర్మాణం group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                ఒకవేళ (r == -ENOENT)
                        కొనసాగు;
                ఒకవేళ (r < 0)
                        తిరిగి r;

                అయితే ((r = fgetgrent_sane(f, &gr)) > 0) {
                        ఒకవేళ (name && !streq_ptr(gr->gr_name, name))
                                కొనసాగు;
                        ఒకవేళ (gid_is_valid(gid) && gr->gr_gid != gid)
                                కొనసాగు;
                        ఒకవేళ (ret)
                                తిరిగి copy_struct_సమూహం(gr, ret);
                        తిరిగి 0;
                }
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        తిరిగి -ESRCH;
}

పూర్ణాంకం sysconf_ngroups_max(శూన్య) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        errno = 0;
        దీర్ఘ ngroups_max = sysconf(_SC_NGROUPS_MAX);
        ఒకవేళ (ngroups_max <= 0)
                తిరిగి errno_or_else(EOPNOTSUPP);
        ఒకవేళ (ngroups_max > గరిష్ఠ_పూర్ణాంకం)
                తిరిగి -ERANGE;
        తిరిగి ngroups_max;
}

ssize_t lookup_groups_లోపల_files(
                అక్షరం * స్థిరరాశి *files,
                స్థిరరాశి అక్షరం *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = ఖాళీ;
        ssize_t n_arr = 0;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(files);
        ప్రతిజ్ఞా(name);
        ప్రతిజ్ఞా(gid_is_valid(gid));

        పూర్ణాంకం ngroups_max = sysconf_ngroups_max();
        ఒకవేళ (ngroups_max < 0)
                తిరిగి ngroups_max;

        ఒకవేళ (!GREEDY_REALLOC(arr, n_arr + 1))
                తిరిగి -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ఖాళీ;
                నిర్మాణం group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                ఒకవేళ (r == -ENOENT)
                        కొనసాగు;
                ఒకవేళ (r < 0)
                        తిరిగి r;

                అయితే ((r = fgetgrent_sane(f, &gr)) > 0) {
                        ఒకవేళ (gid == gr->gr_gid)
                                కొనసాగు;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        ఒకవేళ (gid_list_has(arr, n_arr, gr->gr_gid))
                                కొనసాగు;

                        ఒకవేళ (!strv_కలిగియుంది(gr->gr_mem, name))
                                కొనసాగు;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        ఒకవేళ (n_arr >= ngroups_max)
                                తిరిగి -ENOBUFS;

                        ఒకవేళ (!GREEDY_REALLOC(arr, n_arr + 1))
                                తిరిగి -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        ఒకవేళ (ret)
                *ret = TAKE_PTR(arr);
        తిరిగి n_arr;
}

పూర్ణాంకం getgrouplist_malloc(స్థిరరాశి అక్షరం *user, gid_t gid, gid_t **ret) {
#ఒకవేళ BUILD_STATIC
        తిరిగి lookup_groups_లోపల_files(సమూహం_FILES, user, gid, ret);
#లేకపోతే
        పూర్ణాంకం ngroups_max = sysconf_ngroups_max();
        ఒకవేళ (ngroups_max < 0)
                తిరిగి ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        ఒకవేళ (!gids)
                తిరిగి -ENOMEM;

        పూర్ణాంకం k = ngroups_max;
        ఒకవేళ (getgrouplist(user, gid, gids, &k) < 0)
                తిరిగి -EIO;  /* getgrouplist does not set errno */

        ఒకవేళ (ret)
                *ret = TAKE_PTR(gids);
        తిరిగి k;
#అయితే_ముగింపు
}

పూర్ణాంకం initgroups_wrapper(స్థిరరాశి అక్షరం *user, gid_t gid) {
#ఒకవేళ BUILD_STATIC
        _cleanup_free_ gid_t *groups = ఖాళీ;
        పూర్ణాంకం r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        ఒకవేళ (n_groups <= 0)
                తిరిగి n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        కొరకు (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                ఒకవేళ (r != -EINVAL)
                        ఆపు;
        }

        తిరిగి r;
#లేకపోతే
        తిరిగి RET_NERRNO(initgroups(user, gid));
#అయితే_ముగింపు
}

#ఒకవేళ !BUILD_STATIC

స్థిర size_t getgr_బఫరు_కొలత(శూన్య) {
        దీర్ఘ bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        తిరిగి bufsize <= 0 ? 4096U : (size_t) bufsize;
}

#అయితే_ముగింపు

పూర్ణాంకం getgrnam_malloc(స్థిరరాశి అక్షరం *name, నిర్మాణం group **ret) {
        ఒకవేళ (isempty(name))
                తిరిగి -EINVAL;

#ఒకవేళ BUILD_STATIC
        తిరిగి lookup_grent_లోపల_files(సమూహం_FILES, name, GID_INVALID, ret);
#లేకపోతే
        size_t bufsize = getgr_బఫరు_కొలత();
        పూర్ణాంకం r;

        కొరకు (;;) {
                _cleanup_free_ శూన్య *buf = ఖాళీ;

                /* Silence static analyzers */
                ప్రతిజ్ఞా(bufsize <= SIZE_MAX - ALIGN(పరిమాణం(నిర్మాణం group)));
                buf = malloc0(ALIGN(పరిమాణం(నిర్మాణం group)) + bufsize);
                ఒకవేళ (!buf)
                        తిరిగి -ENOMEM;

                నిర్మాణం group *gr = ఖాళీ;
                r = getgrnam_r(name, buf, (అక్షరం*) buf + ALIGN(పరిమాణం(నిర్మాణం group)), bufsize, &gr);
                ఒకవేళ (r == 0) {
                        ఒకవేళ (gr) {
                                ఒకవేళ (ret)
                                        *ret = TAKE_PTR(buf);
                                తిరిగి 0;
                        }

                        తిరిగి -ESRCH;
                }

                ప్రతిజ్ఞా(r > 0);

                ఒకవేళ (errno_is_user_doesnt_exist(r))
                        తిరిగి -ESRCH;
                ఒకవేళ (r != ERANGE)
                        తిరిగి -r;

                ఒకవేళ (bufsize > SIZE_MAX/2 - ALIGN(పరిమాణం(నిర్మాణం group)))
                        తిరిగి -ENOMEM;
                bufsize *= 2;
        }
#అయితే_ముగింపు
}

పూర్ణాంకం getgrgid_malloc(gid_t gid, నిర్మాణం group **ret) {
        ఒకవేళ (!gid_is_valid(gid))
                తిరిగి -EINVAL;

#ఒకవేళ BUILD_STATIC
        తిరిగి lookup_grent_లోపల_files(సమూహం_FILES, /* name= */ ఖాళీ, gid, ret);
#లేకపోతే
        size_t bufsize = getgr_బఫరు_కొలత();
        పూర్ణాంకం r;

        కొరకు (;;) {
                _cleanup_free_ శూన్య *buf = ఖాళీ;

                /* Silence static analyzers */
                ప్రతిజ్ఞా(bufsize <= SIZE_MAX - ALIGN(పరిమాణం(నిర్మాణం group)));
                buf = malloc0(ALIGN(పరిమాణం(నిర్మాణం group)) + bufsize);
                ఒకవేళ (!buf)
                        తిరిగి -ENOMEM;

                నిర్మాణం group *gr = ఖాళీ;
                r = getgrgid_r(gid, buf, (అక్షరం*) buf + ALIGN(పరిమాణం(నిర్మాణం group)), bufsize, &gr);
                ఒకవేళ (r == 0) {
                        ఒకవేళ (gr) {
                                ఒకవేళ (ret)
                                        *ret = TAKE_PTR(buf);
                                తిరిగి 0;
                        }

                        తిరిగి -ESRCH;
                }

                ప్రతిజ్ఞా(r > 0);

                ఒకవేళ (errno_is_user_doesnt_exist(r))
                        తిరిగి -ESRCH;
                ఒకవేళ (r != ERANGE)
                        తిరిగి -r;

                ఒకవేళ (bufsize > SIZE_MAX/2 - ALIGN(పరిమాణం(నిర్మాణం group)))
                        తిరిగి -ENOMEM;
                bufsize *= 2;
        }
#అయితే_ముగింపు
}
