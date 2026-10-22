/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ <grp.h>
#যদি ENABLE_GSHADOW
#  include <gshadow.h>
#endif
#অন্তর্ভাৱ <pwd.h>
#অন্তর্ভাৱ <shadow.h>

#অন্তর্ভাৱ "forward.h"
#অন্তর্ভাৱ "errno-util.h"

/* Users managed by systemd-homed. See https://systemd.io/UIDS-GIDS for details
 * how this range fits into the rest of the world. */
#define HOME_UID_MIN ((uid_t) 60001)
#define HOME_UID_MAX ((uid_t) 60513)

/* Users mapped from host into a container */
#define MAP_UID_MIN ((uid_t) 60514)
#define MAP_UID_MAX ((uid_t) 60577)

/* A helper to print an error message when user or group resolution fails. */
ধ্রুৱ ৱর্ণ* strerror_user(পূর্ণাঙ্ক errnum, ৱর্ণ *buf, size_t buflen);
#define STRERROR_USER(errnum) strerror_user(errnum, (ৱর্ণ[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)
ধ্রুৱ ৱর্ণ* strerror_group(পূর্ণাঙ্ক errnum, ৱর্ণ *buf, size_t buflen);
#define STRERROR_GROUP(errnum) strerror_group(errnum, (ৱর্ণ[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)

স্থির অন্তঃস্থ bool ERRNO_IS_NEG_BAD_ACCOUNT(intmax_t r) {
        ঘূৰাই IN_SET(r,
                      -ESRCH,
                      -ENOEXEC);
}
_DEFINE_ABS_WRAPPER(BAD_ACCOUNT);

bool uid_is_valid(uid_t uid) _const_;

স্থির অন্তঃস্থ bool gid_is_valid(gid_t gid) {
        ঘূৰাই uid_is_valid((uid_t) gid);
}

পূর্ণাঙ্ক parse_uid(ধ্রুৱ ৱর্ণ *s, uid_t* ret_uid);
পূর্ণাঙ্ক parse_uid_range(ধ্রুৱ ৱর্ণ *s, uid_t *ret_lower, uid_t *ret_upper);

স্থির অন্তঃস্থ পূর্ণাঙ্ক parse_gid(ধ্রুৱ ৱর্ণ *s, gid_t *ret_gid) {
        ঘূৰাই parse_uid(s, (uid_t*) ret_gid);
}

ৱর্ণ* getlogname_malloc(শূন্য);
ৱর্ণ* getusername_malloc(শূন্য);

ধ্রুৱ ৱর্ণ* default_মূল_shell_at(পূর্ণাঙ্ক rfd);
ধ্রুৱ ৱর্ণ* default_মূল_shell(ধ্রুৱ ৱর্ণ *মূল);

bool is_nologin_shell(ধ্রুৱ ৱর্ণ *shell) _pure_;
bool shell_is_placeholder(ধ্রুৱ ৱর্ণ *shell) _pure_;

প্রকারনির্দেশ গণনা UserCredsFlags {
        USER_CREDS_PREFER_NSS           = 1 << 0,  /* if set, only synthesize user records if database lacks them. Normally we bypass the userdb entirely for the records we can synthesize */
        USER_CREDS_ALLOW_MISSING        = 1 << 1,  /* if a numeric UID string is resolved, be OK if there's no record for it */
        USER_CREDS_CLEAN                = 1 << 2,  /* try to clean up shell and home fields with invalid data */
        USER_CREDS_SUPPRESS_PLACEHOLDER = 1 << 3,  /* suppress home and/or shell fields if value is placeholder (root/empty/nologin) */
} UserCredsFlags;

পূর্ণাঙ্ক get_user_creds(
                ধ্রুৱ ৱর্ণ *username,
                UserCredsFlags flags,
                ৱর্ণ **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                ৱর্ণ **ret_home,
                ৱর্ণ **ret_shell);
পূর্ণাঙ্ক get_group_creds(ধ্রুৱ ৱর্ণ *groupname, UserCredsFlags flags, ৱর্ণ **ret_name, gid_t *ret_gid);

ৱর্ণ* uid_to_name(uid_t uid);
ৱর্ণ* gid_to_name(gid_t gid);

পূর্ণাঙ্ক in_gid(gid_t gid);
পূর্ণাঙ্ক in_group(ধ্রুৱ ৱর্ণ *name);

পূর্ণাঙ্ক merge_gid_lists(ধ্রুৱ gid_t *list1, size_t size1, ধ্রুৱ gid_t *list2, size_t size2, gid_t **ret);
পূর্ণাঙ্ক getgroups_alloc(gid_t **ret);

পূর্ণাঙ্ক get_home_dir(ৱর্ণ **ret);
পূর্ণাঙ্ক get_shell(ৱর্ণ **ret);

পূর্ণাঙ্ক fully_set_uid_gid(uid_t uid, gid_t gid, ধ্রুৱ gid_t supplementary_gids[], size_t n_supplementary_gids);
স্থির অন্তঃস্থ পূর্ণাঙ্ক reset_uid_gid(শূন্য) {
        ঘূৰাই fully_set_uid_gid(0, 0, রিক্ত, 0);
}

পূর্ণাঙ্ক take_etc_passwd_lock(ধ্রুৱ ৱর্ণ *মূল);

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
#define UID_MAPPED_মূল ((uid_t) (INT32_MAX-1))
#define GID_MAPPED_মূল ((gid_t) (INT32_MAX-1))

#define ETC_PASSWD_LOCK_FILENAME ".pwd.lock"
#define ETC_PASSWD_LOCK_PATH "/etc/" ETC_PASSWD_LOCK_FILENAME

/* The following macros add 1 when converting things, since UID 0 is a valid UID, while the pointer
 * NULL is special */
#define PTR_TO_UID(p) ((uid_t) (((uintptr_t) (p))-1))
#define UID_TO_PTR(u) ((শূন্য*) (((uintptr_t) (u))+1))

#define PTR_TO_GID(p) ((gid_t) (((uintptr_t) (p))-1))
#define GID_TO_PTR(u) ((শূন্য*) (((uintptr_t) (u))+1))

প্রকারনির্দেশ গণনা ValidUserFlags {
        VALID_USER_RELAX         = 1 << 0,
        VALID_USER_WARN          = 1 << 1,
        VALID_USER_ALLOW_NUMERIC = 1 << 2,
} ValidUserFlags;

bool valid_user_group_name(ধ্রুৱ ৱর্ণ *u, ValidUserFlags flags);
bool valid_gecos(ধ্রুৱ ৱর্ণ *d);
ৱর্ণ* mangle_gecos(ধ্রুৱ ৱর্ণ *d);
bool valid_home(ধ্রুৱ ৱর্ণ *p);
bool valid_shell(ধ্রুৱ ৱর্ণ *p);

পূর্ণাঙ্ক maybe_setgroups(size_t size, ধ্রুৱ gid_t *list);

bool synthesize_nobody(শূন্য);

পূর্ণাঙ্ক fgetpwent_sane(FILE *stream, সংরচনা passwd **pw);
পূর্ণাঙ্ক fgetspent_sane(FILE *stream, সংরচনা spwd **sp);
পূর্ণাঙ্ক fgetgrent_sane(FILE *stream, সংরচনা group **gr);
পূর্ণাঙ্ক putpwent_sane(ধ্রুৱ সংরচনা passwd *pw, FILE *stream);
পূর্ণাঙ্ক putspent_sane(ধ্রুৱ সংরচনা spwd *sp, FILE *stream);
পূর্ণাঙ্ক putgrent_sane(ধ্রুৱ সংরচনা group *gr, FILE *stream);
#যদি ENABLE_GSHADOW
পূর্ণাঙ্ক fgetsgent_sane(FILE *stream, সংরচনা sgrp **sg);
পূর্ণাঙ্ক putsgent_sane(ধ্রুৱ সংরচনা sgrp *sg, FILE *stream);
#endif

পূর্ণাঙ্ক is_this_me(ধ্রুৱ ৱর্ণ *username);

ধ্রুৱ ৱর্ণ* get_home_মূল(শূন্য);

স্থির অন্তঃস্থ bool hashed_password_is_locked_or_invalid(ধ্রুৱ ৱর্ণ *password) {
        ঘূৰাই password && password[0] != '$';
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

পূর্ণাঙ্ক sysconf_ngroups_max(শূন্য);

পূর্ণাঙ্ক lookup_pwent_in_files(
                ৱর্ণ * ধ্রুৱ *files,
                ধ্রুৱ ৱর্ণ *name,
                uid_t uid,
                সংরচনা passwd **ret);
পূর্ণাঙ্ক lookup_grent_in_files(
                ৱর্ণ * ধ্রুৱ *files,
                ধ্রুৱ ৱর্ণ *name,
                gid_t gid,
                সংরচনা group **ret);

ssize_t lookup_groups_in_files(
                ৱর্ণ * ধ্রুৱ *files,
                ধ্রুৱ ৱর্ণ *name,
                gid_t gid,
                gid_t **ret);
পূর্ণাঙ্ক getgrouplist_malloc(ধ্রুৱ ৱর্ণ *user, gid_t gid, gid_t **ret);
পূর্ণাঙ্ক initgroups_wrapper(ধ্রুৱ ৱর্ণ *user, gid_t gid);

পূর্ণাঙ্ক getpwuid_malloc(uid_t uid, সংরচনা passwd **ret);
পূর্ণাঙ্ক getpwnam_malloc(ধ্রুৱ ৱর্ণ *name, সংরচনা passwd **ret);

পূর্ণাঙ্ক getgrnam_malloc(ধ্রুৱ ৱর্ণ *name, সংরচনা group **ret);
পূর্ণাঙ্ক getgrgid_malloc(gid_t gid, সংরচনা group **ret);
