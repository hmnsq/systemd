/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <grp.h>
#જો ENABLE_GSHADOW
#  include <gshadow.h>
#endif
#અન્તર્ભાવ <pwd.h>
#અન્તર્ભાવ <shadow.h>

#અન્તર્ભાવ "forward.h"
#અન્તર્ભાવ "errno-util.h"

/* Users managed by systemd-homed. See https://systemd.io/UIDS-GIDS for details
 * how this range fits into the rest of the world. */
#define HOME_UID_MIN ((uid_t) 60001)
#define HOME_UID_MAX ((uid_t) 60513)

/* Users mapped from host into a container */
#define MAP_UID_MIN ((uid_t) 60514)
#define MAP_UID_MAX ((uid_t) 60577)

/* A helper to print an error message when user or group resolution fails. */
અચલ અક્ષર* strerror_user(પૂર્ણાંક errnum, અક્ષર *buf, size_t buflen);
#define STRERROR_USER(errnum) strerror_user(errnum, (અક્ષર[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)
અચલ અક્ષર* strerror_group(પૂર્ણાંક errnum, અક્ષર *buf, size_t buflen);
#define STRERROR_GROUP(errnum) strerror_group(errnum, (અક્ષર[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)

સ્થિર અંતઃરેખ bool ERRNO_IS_NEG_BAD_ACCOUNT(intmax_t r) {
        પાછા IN_SET(r,
                      -ESRCH,
                      -ENOEXEC);
}
_DEFINE_ABS_WRAPPER(BAD_ACCOUNT);

bool uid_is_valid(uid_t uid) _const_;

સ્થિર અંતઃરેખ bool gid_is_valid(gid_t gid) {
        પાછા uid_is_valid((uid_t) gid);
}

પૂર્ણાંક parse_uid(અચલ અક્ષર *s, uid_t* ret_uid);
પૂર્ણાંક parse_uid_range(અચલ અક્ષર *s, uid_t *ret_lower, uid_t *ret_upper);

સ્થિર અંતઃરેખ પૂર્ણાંક parse_gid(અચલ અક્ષર *s, gid_t *ret_gid) {
        પાછા parse_uid(s, (uid_t*) ret_gid);
}

અક્ષર* getlogname_malloc(શૂન્ય);
અક્ષર* getusername_malloc(શૂન્ય);

અચલ અક્ષર* default_મૂળ_shell_at(પૂર્ણાંક rfd);
અચલ અક્ષર* default_મૂળ_shell(અચલ અક્ષર *મૂળ);

bool is_nologin_shell(અચલ અક્ષર *shell) _pure_;
bool shell_is_placeholder(અચલ અક્ષર *shell) _pure_;

પ્રકારવ્યાખ્યા ગણના UserCredsFlags {
        USER_CREDS_PREFER_NSS           = 1 << 0,  /* if set, only synthesize user records if database lacks them. Normally we bypass the userdb entirely for the records we can synthesize */
        USER_CREDS_ALLOW_MISSING        = 1 << 1,  /* if a numeric UID string is resolved, be OK if there's no record for it */
        USER_CREDS_CLEAN                = 1 << 2,  /* try to clean up shell and home fields with invalid data */
        USER_CREDS_SUPPRESS_PLACEHOLDER = 1 << 3,  /* suppress home and/or shell fields if value is placeholder (root/empty/nologin) */
} UserCredsFlags;

પૂર્ણાંક get_user_creds(
                અચલ અક્ષર *username,
                UserCredsFlags flags,
                અક્ષર **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                અક્ષર **ret_home,
                અક્ષર **ret_shell);
પૂર્ણાંક get_group_creds(અચલ અક્ષર *groupname, UserCredsFlags flags, અક્ષર **ret_name, gid_t *ret_gid);

અક્ષર* uid_to_name(uid_t uid);
અક્ષર* gid_to_name(gid_t gid);

પૂર્ણાંક in_gid(gid_t gid);
પૂર્ણાંક in_group(અચલ અક્ષર *name);

પૂર્ણાંક merge_gid_lists(અચલ gid_t *list1, size_t size1, અચલ gid_t *list2, size_t size2, gid_t **ret);
પૂર્ણાંક getgroups_alloc(gid_t **ret);

પૂર્ણાંક get_home_dir(અક્ષર **ret);
પૂર્ણાંક get_shell(અક્ષર **ret);

પૂર્ણાંક fully_set_uid_gid(uid_t uid, gid_t gid, અચલ gid_t supplementary_gids[], size_t n_supplementary_gids);
સ્થિર અંતઃરેખ પૂર્ણાંક reset_uid_gid(શૂન્ય) {
        પાછા fully_set_uid_gid(0, 0, ખાલી, 0);
}

પૂર્ણાંક take_etc_passwd_lock(અચલ અક્ષર *મૂળ);

#define UID_INVALID ((uid_t) -1)
#define GID_INVALID ((gid_t) -1)

#define UID_NOBODY ((uid_t) 65534U)
#define GID_NOBODY ((gid_t) 65534U)

/* Conventional size of a user-namespace UID/GID delegation block (64K).
 * Untyped so it can be used in both UID and GID contexts without casts. */
#define USERNS_RANGE_SIZE 0x10000U

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
#define UID_MAPPED_મૂળ ((uid_t) (INT32_MAX-1))
#define GID_MAPPED_મૂળ ((gid_t) (INT32_MAX-1))

#define ETC_PASSWD_LOCK_FILENAME ".pwd.lock"
#define ETC_PASSWD_LOCK_PATH "/etc/" ETC_PASSWD_LOCK_FILENAME

/* The following macros add 1 when converting things, since UID 0 is a valid UID, while the pointer
 * NULL is special */
#define PTR_TO_UID(p) ((uid_t) (((uintptr_t) (p))-1))
#define UID_TO_PTR(u) ((શૂન્ય*) (((uintptr_t) (u))+1))

#define PTR_TO_GID(p) ((gid_t) (((uintptr_t) (p))-1))
#define GID_TO_PTR(u) ((શૂન્ય*) (((uintptr_t) (u))+1))

પ્રકારવ્યાખ્યા ગણના ValidUserFlags {
        VALID_USER_RELAX         = 1 << 0,
        VALID_USER_WARN          = 1 << 1,
        VALID_USER_ALLOW_NUMERIC = 1 << 2,
} ValidUserFlags;

bool valid_user_group_name(અચલ અક્ષર *u, ValidUserFlags flags);
bool valid_gecos(અચલ અક્ષર *d);
અક્ષર* mangle_gecos(અચલ અક્ષર *d);
bool valid_home(અચલ અક્ષર *p);
bool valid_shell(અચલ અક્ષર *p);

પૂર્ણાંક maybe_setgroups(size_t size, અચલ gid_t *list);

bool synthesize_nobody(શૂન્ય);

પૂર્ણાંક fgetpwent_sane(FILE *stream, સંરચના passwd **pw);
પૂર્ણાંક fgetspent_sane(FILE *stream, સંરચના spwd **sp);
પૂર્ણાંક fgetgrent_sane(FILE *stream, સંરચના group **gr);
પૂર્ણાંક putpwent_sane(અચલ સંરચના passwd *pw, FILE *stream);
પૂર્ણાંક putspent_sane(અચલ સંરચના spwd *sp, FILE *stream);
પૂર્ણાંક putgrent_sane(અચલ સંરચના group *gr, FILE *stream);
#જો ENABLE_GSHADOW
પૂર્ણાંક fgetsgent_sane(FILE *stream, સંરચના sgrp **sg);
પૂર્ણાંક putsgent_sane(અચલ સંરચના sgrp *sg, FILE *stream);
#endif

પૂર્ણાંક is_this_me(અચલ અક્ષર *username);

અચલ અક્ષર* get_home_મૂળ(શૂન્ય);

સ્થિર અંતઃરેખ bool hashed_password_is_locked_or_invalid(અચલ અક્ષર *password) {
        પાછા password && password[0] != '$';
}

/* Places where we will try to load account data from */
#define PASSWD_FILES STRV_MAKE("/etc/passwd", "/usr/lib/passwd")
#define GROUP_FILES STRV_MAKE("/etc/group", "/usr/lib/group")

/* A locked *and* invalid password for "struct spwd"'s .sp_pwdp and "struct passwd"'s .pw_passwd field */
#define PASSWORD_LOCKED_AND_INVALID "!*"

/* A password indicating "look in shadow file, please!" for "struct passwd"'s .pw_passwd */
#define PASSWORD_SEE_SHADOW "x"

/* A password indicating "hey, no password required for login" */
#define PASSWORD_NONE ""

/* Used by sysusers to indicate that the password should be filled in by firstboot.
 * Also see https://github.com/systemd/systemd/pull/24680#pullrequestreview-1439464325.
 */
#define PASSWORD_UNPROVISIONED "!unprovisioned"

પૂર્ણાંક sysconf_ngroups_max(શૂન્ય);

પૂર્ણાંક lookup_pwent_in_files(
                અક્ષર * અચલ *files,
                અચલ અક્ષર *name,
                uid_t uid,
                સંરચના passwd **ret);
પૂર્ણાંક lookup_grent_in_files(
                અક્ષર * અચલ *files,
                અચલ અક્ષર *name,
                gid_t gid,
                સંરચના group **ret);

ssize_t lookup_groups_in_files(
                અક્ષર * અચલ *files,
                અચલ અક્ષર *name,
                gid_t gid,
                gid_t **ret);
પૂર્ણાંક getgrouplist_malloc(અચલ અક્ષર *user, gid_t gid, gid_t **ret);
પૂર્ણાંક initgroups_wrapper(અચલ અક્ષર *user, gid_t gid);

પૂર્ણાંક getpwuid_malloc(uid_t uid, સંરચના passwd **ret);
પૂર્ણાંક getpwnam_malloc(અચલ અક્ષર *name, સંરચના passwd **ret);

પૂર્ણાંક getgrnam_malloc(અચલ અક્ષર *name, સંરચના group **ret);
પૂર્ણાંક getgrgid_malloc(gid_t gid, સંરચના group **ret);
