/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <grp.h>
#ఒకవేళ ENABLE_GSHADOW
#  include <gshadow.h>
#అయితే_ముగింపు
#అన్తర్భావ <pwd.h>
#అన్తర్భావ <shadow.h>

#అన్తర్భావ "forward.h"
#అన్తర్భావ "errno-util.h"

/* Users managed by systemd-homed. See https://systemd.io/UIDS-GIDS for details
 * how this range fits into the rest of the world. */
#నిర్వచించు HOME_UID_MIN ((uid_t) 60001)
#నిర్వచించు HOME_UID_MAX ((uid_t) 60513)

/* Users mapped from host into a container */
#నిర్వచించు MAP_UID_MIN ((uid_t) 60514)
#నిర్వచించు MAP_UID_MAX ((uid_t) 60577)

/* A helper to print an error message when user or group resolution fails. */
స్థిరరాశి అక్షరం* strerror_user(పూర్ణాంకం errnum, అక్షరం *buf, size_t buflen);
#నిర్వచించు STRERROR_USER(errnum) strerror_user(errnum, (అక్షరం[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)
స్థిరరాశి అక్షరం* strerror_సమూహం(పూర్ణాంకం errnum, అక్షరం *buf, size_t buflen);
#నిర్వచించు STRERROR_సమూహం(errnum) strerror_సమూహం(errnum, (అక్షరం[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)

స్థిర అంతఃవరుస bool ERRNO_IS_NEG_BAD_ACCOUNT(intmax_t r) {
        తిరిగి లోపల_సమితి(r,
                      -ESRCH,
                      -ENOEXEC);
}
_DEFINE_ABS_WRAPPER(BAD_ACCOUNT);

bool uid_is_valid(uid_t uid) _const_;

స్థిర అంతఃవరుస bool gid_is_valid(gid_t gid) {
        తిరిగి uid_is_valid((uid_t) gid);
}

పూర్ణాంకం parse_uid(స్థిరరాశి అక్షరం *s, uid_t* ret_uid);
పూర్ణాంకం parse_uid_range(స్థిరరాశి అక్షరం *s, uid_t *ret_క్రింది, uid_t *ret_పై);

స్థిర అంతఃవరుస పూర్ణాంకం parse_gid(స్థిరరాశి అక్షరం *s, gid_t *ret_gid) {
        తిరిగి parse_uid(s, (uid_t*) ret_gid);
}

అక్షరం* getlogname_malloc(శూన్య);
అక్షరం* getusername_malloc(శూన్య);

స్థిరరాశి అక్షరం* default_మూలం_shell_at(పూర్ణాంకం rfd);
స్థిరరాశి అక్షరం* default_మూలం_shell(స్థిరరాశి అక్షరం *మూలం);

bool is_nologin_shell(స్థిరరాశి అక్షరం *shell) శుద్ధ;
bool shell_is_placeholder(స్థిరరాశి అక్షరం *shell) శుద్ధ;

రకనిర్వచనం గణన UserCredsFlags {
        USER_CREDS_PREFER_NSS           = 1 << 0,  /* if set, only synthesize user records if database lacks them. Normally we bypass the userdb entirely for the records we can synthesize */
        USER_CREDS_ALLOW_MISSING        = 1 << 1,  /* if a numeric UID string is resolved, be OK if there's no record for it */
        USER_CREDS_CLEAN                = 1 << 2,  /* try to clean up shell and home fields with invalid data */
        USER_CREDS_SUPPRESS_PLACEHOLDER = 1 << 3,  /* suppress home and/or shell fields if value is placeholder (root/empty/nologin) */
} UserCredsFlags;

పూర్ణాంకం get_user_creds(
                స్థిరరాశి అక్షరం *username,
                UserCredsFlags flags,
                అక్షరం **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                అక్షరం **ret_home,
                అక్షరం **ret_shell);
పూర్ణాంకం get_సమూహం_creds(స్థిరరాశి అక్షరం *groupname, UserCredsFlags flags, అక్షరం **ret_name, gid_t *ret_gid);

అక్షరం* uid_గమ్యం_name(uid_t uid);
అక్షరం* gid_గమ్యం_name(gid_t gid);

పూర్ణాంకం లోపల_gid(gid_t gid);
పూర్ణాంకం లోపల_సమూహం(స్థిరరాశి అక్షరం *name);

పూర్ణాంకం విలీనం_gid_lists(స్థిరరాశి gid_t *list1, size_t size1, స్థిరరాశి gid_t *list2, size_t size2, gid_t **ret);
పూర్ణాంకం getgroups_alloc(gid_t **ret);

పూర్ణాంకం get_home_dir(అక్షరం **ret);
పూర్ణాంకం get_shell(అక్షరం **ret);

పూర్ణాంకం fully_సమితి_uid_gid(uid_t uid, gid_t gid, స్థిరరాశి gid_t supplementary_gids[], size_t n_supplementary_gids);
స్థిర అంతఃవరుస పూర్ణాంకం reset_uid_gid(శూన్య) {
        తిరిగి fully_సమితి_uid_gid(0, 0, ఖాళీ, 0);
}

పూర్ణాంకం take_etc_passwd_తాళం(స్థిరరాశి అక్షరం *మూలం);

#నిర్వచించు UID_INVALID ((uid_t) -1)
#నిర్వచించు GID_INVALID ((gid_t) -1)

#నిర్వచించు UID_NOBODY ((uid_t) 65534U)
#నిర్వచించు GID_NOBODY ((gid_t) 65534U)

/* Conventional size of a user-namespace UID/GID delegation block (64K).
 * Untyped so it can be used in both UID and GID contexts without casts. */
#నిర్వచించు USERNS_RANGE_కొలత 0x10000U

/* If REMOUNT_IDMAPPING_HOST_ROOT is set for remount_idmap() we'll include a mapping here that maps the host
 * root user accessing the idmapped mount to the this user ID on the backing fs. This is the last valid UID in
 * the *signed* 32-bit range. You might wonder why precisely use this specific UID for this purpose? Well, we
 * definitely cannot use the first 0…65536 UIDs for that, since in most cases that's precisely the file range
 * we intend to map to some high UID range, and since UID mappings have to be bijective we thus cannot use
 * them at all. Furthermore the UID range beyond INT32_MAX (i.e. the range above the signed 32-bit range) is
 * icky, since many APIs cannot use it (example: setfsuid() returns the old UID as signed integer). Following
 * our usual logic of assigning a 16-bit UID range to each container, so that the upper 16-bit of a 32-bit UID
 * value indicate kind of a "container ID" and the lower 16-bit map directly to the intended user you can read
 * this specific UID as the "nobody" user of the container with ID 0x7FFF, which is kinda nice. */
#నిర్వచించు UID_MAPPED_మూలం ((uid_t) (INT32_MAX-1))
#నిర్వచించు GID_MAPPED_మూలం ((gid_t) (INT32_MAX-1))

#నిర్వచించు ETC_PASSWD_తాళం_FILENAME ".pwd.lock"
#నిర్వచించు ETC_PASSWD_తాళం_మార్గం "/etc/" ETC_PASSWD_తాళం_FILENAME

/* The following macros add 1 when converting things, since UID 0 is a valid UID, while the pointer
 * NULL is special */
#నిర్వచించు PTR_గమ్యం_UID(p) ((uid_t) (((uintptr_t) (p))-1))
#నిర్వచించు UID_గమ్యం_PTR(u) ((శూన్య*) (((uintptr_t) (u))+1))

#నిర్వచించు PTR_గమ్యం_GID(p) ((gid_t) (((uintptr_t) (p))-1))
#నిర్వచించు GID_గమ్యం_PTR(u) ((శూన్య*) (((uintptr_t) (u))+1))

రకనిర్వచనం గణన ValidUserFlags {
        VALID_USER_RELAX         = 1 << 0,
        VALID_USER_WARN          = 1 << 1,
        VALID_USER_ALLOW_NUMERIC = 1 << 2,
} ValidUserFlags;

bool valid_user_సమూహం_name(స్థిరరాశి అక్షరం *u, ValidUserFlags flags);
bool valid_gecos(స్థిరరాశి అక్షరం *d);
అక్షరం* mangle_gecos(స్థిరరాశి అక్షరం *d);
bool valid_home(స్థిరరాశి అక్షరం *p);
bool valid_shell(స్థిరరాశి అక్షరం *p);

పూర్ణాంకం maybe_setgroups(size_t కొలత, స్థిరరాశి gid_t *list);

bool synthesize_nobody(శూన్య);

పూర్ణాంకం fgetpwent_sane(FILE *ప్రవాహం, నిర్మాణం passwd **pw);
పూర్ణాంకం fgetspent_sane(FILE *ప్రవాహం, నిర్మాణం spwd **sp);
పూర్ణాంకం fgetgrent_sane(FILE *ప్రవాహం, నిర్మాణం group **gr);
పూర్ణాంకం putpwent_sane(స్థిరరాశి నిర్మాణం passwd *pw, FILE *ప్రవాహం);
పూర్ణాంకం putspent_sane(స్థిరరాశి నిర్మాణం spwd *sp, FILE *ప్రవాహం);
పూర్ణాంకం putgrent_sane(స్థిరరాశి నిర్మాణం group *gr, FILE *ప్రవాహం);
#ఒకవేళ ENABLE_GSHADOW
పూర్ణాంకం fgetsgent_sane(FILE *ప్రవాహం, నిర్మాణం sgrp **sg);
పూర్ణాంకం putsgent_sane(స్థిరరాశి నిర్మాణం sgrp *sg, FILE *ప్రవాహం);
#అయితే_ముగింపు

పూర్ణాంకం is_this_me(స్థిరరాశి అక్షరం *username);

స్థిరరాశి అక్షరం* get_home_మూలం(శూన్య);

స్థిర అంతఃవరుస bool hashed_password_is_locked_or_invalid(స్థిరరాశి అక్షరం *password) {
        తిరిగి password && password[0] != '$';
}

/* Places where we will try to load account data from */
#నిర్వచించు PASSWD_FILES STRV_MAKE("/etc/passwd", "/usr/lib/passwd")
#నిర్వచించు సమూహం_FILES STRV_MAKE("/etc/group", "/usr/lib/group")

/* A locked *and* invalid password for "struct spwd"'s .sp_pwdp and "struct passwd"'s .pw_passwd field */
#నిర్వచించు PASSWORD_LOCKED_AND_INVALID "!*"

/* A password indicating "look in shadow file, please!" for "struct passwd"'s .pw_passwd */
#నిర్వచించు PASSWORD_SEE_SHADOW "x"

/* A password indicating "hey, no password required for login" */
#నిర్వచించు PASSWORD_NONE ""

/* Used by sysusers to indicate that the password should be filled in by firstboot.
 * Also see https://github.com/systemd/systemd/pull/24680#pullrequestreview-1439464325.
 */
#నిర్వచించు PASSWORD_UNPROVISIONED "!unprovisioned"

పూర్ణాంకం sysconf_ngroups_max(శూన్య);

పూర్ణాంకం lookup_pwent_లోపల_files(
                అక్షరం * స్థిరరాశి *files,
                స్థిరరాశి అక్షరం *name,
                uid_t uid,
                నిర్మాణం passwd **ret);
పూర్ణాంకం lookup_grent_లోపల_files(
                అక్షరం * స్థిరరాశి *files,
                స్థిరరాశి అక్షరం *name,
                gid_t gid,
                నిర్మాణం group **ret);

ssize_t lookup_groups_లోపల_files(
                అక్షరం * స్థిరరాశి *files,
                స్థిరరాశి అక్షరం *name,
                gid_t gid,
                gid_t **ret);
పూర్ణాంకం getgrouplist_malloc(స్థిరరాశి అక్షరం *user, gid_t gid, gid_t **ret);
పూర్ణాంకం initgroups_wrapper(స్థిరరాశి అక్షరం *user, gid_t gid);

పూర్ణాంకం getpwuid_malloc(uid_t uid, నిర్మాణం passwd **ret);
పూర్ణాంకం getpwnam_malloc(స్థిరరాశి అక్షరం *name, నిర్మాణం passwd **ret);

పూర్ణాంకం getgrnam_malloc(స్థిరరాశి అక్షరం *name, నిర్మాణం group **ret);
పూర్ణాంకం getgrgid_malloc(gid_t gid, నిర్మాణం group **ret);
