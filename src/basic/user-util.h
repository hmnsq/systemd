/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <grp.h>
#எனில் ENABLE_GSHADOW
#  include <gshadow.h>
#endif
#அந்தர்பாவ <pwd.h>
#அந்தர்பாவ <shadow.h>

#அந்தர்பாவ "forward.h"
#அந்தர்பாவ "errno-util.h"

/* Users managed by systemd-homed. See https://systemd.io/UIDS-GIDS for details
 * how this range fits into the rest of the world. */
#define HOME_UID_MIN ((uid_t) 60001)
#define HOME_UID_MAX ((uid_t) 60513)

/* Users mapped from host into a container */
#define MAP_UID_MIN ((uid_t) 60514)
#define MAP_UID_MAX ((uid_t) 60577)

/* A helper to print an error message when user or group resolution fails. */
மாறிலி எழுத்து* strerror_user(முழு errnum, எழுத்து *buf, size_t buflen);
#define STRERROR_USER(errnum) strerror_user(errnum, (எழுத்து[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)
மாறிலி எழுத்து* strerror_group(முழு errnum, எழுத்து *buf, size_t buflen);
#define STRERROR_GROUP(errnum) strerror_group(errnum, (எழுத்து[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)

நிலையான உள்வரி bool ERRNO_IS_NEG_BAD_ACCOUNT(intmax_t r) {
        திரும்பு IN_SET(r,
                      -ESRCH,
                      -ENOEXEC);
}
_DEFINE_ABS_WRAPPER(BAD_ACCOUNT);

bool uid_is_valid(uid_t uid) _const_;

நிலையான உள்வரி bool gid_is_valid(gid_t gid) {
        திரும்பு uid_is_valid((uid_t) gid);
}

முழு parse_uid(மாறிலி எழுத்து *s, uid_t* ret_uid);
முழு parse_uid_range(மாறிலி எழுத்து *s, uid_t *ret_lower, uid_t *ret_upper);

நிலையான உள்வரி முழு parse_gid(மாறிலி எழுத்து *s, gid_t *ret_gid) {
        திரும்பு parse_uid(s, (uid_t*) ret_gid);
}

எழுத்து* getlogname_malloc(வெறுமை);
எழுத்து* getusername_malloc(வெறுமை);

மாறிலி எழுத்து* default_வேர்_shell_at(முழு rfd);
மாறிலி எழுத்து* default_வேர்_shell(மாறிலி எழுத்து *வேர்);

bool is_nologin_shell(மாறிலி எழுத்து *shell) _pure_;
bool shell_is_placeholder(மாறிலி எழுத்து *shell) _pure_;

வகைவரை எண்ணிக்கை UserCredsFlags {
        USER_CREDS_PREFER_NSS           = 1 << 0,  /* if set, only synthesize user records if database lacks them. Normally we bypass the userdb entirely for the records we can synthesize */
        USER_CREDS_ALLOW_MISSING        = 1 << 1,  /* if a numeric UID string is resolved, be OK if there's no record for it */
        USER_CREDS_CLEAN                = 1 << 2,  /* try to clean up shell and home fields with invalid data */
        USER_CREDS_SUPPRESS_PLACEHOLDER = 1 << 3,  /* suppress home and/or shell fields if value is placeholder (root/empty/nologin) */
} UserCredsFlags;

முழு get_user_creds(
                மாறிலி எழுத்து *username,
                UserCredsFlags flags,
                எழுத்து **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                எழுத்து **ret_home,
                எழுத்து **ret_shell);
முழு get_group_creds(மாறிலி எழுத்து *groupname, UserCredsFlags flags, எழுத்து **ret_name, gid_t *ret_gid);

எழுத்து* uid_to_name(uid_t uid);
எழுத்து* gid_to_name(gid_t gid);

முழு in_gid(gid_t gid);
முழு in_group(மாறிலி எழுத்து *name);

முழு merge_gid_lists(மாறிலி gid_t *list1, size_t size1, மாறிலி gid_t *list2, size_t size2, gid_t **ret);
முழு getgroups_alloc(gid_t **ret);

முழு get_home_dir(எழுத்து **ret);
முழு get_shell(எழுத்து **ret);

முழு fully_set_uid_gid(uid_t uid, gid_t gid, மாறிலி gid_t supplementary_gids[], size_t n_supplementary_gids);
நிலையான உள்வரி முழு reset_uid_gid(வெறுமை) {
        திரும்பு fully_set_uid_gid(0, 0, வெற்று, 0);
}

முழு take_etc_passwd_lock(மாறிலி எழுத்து *வேர்);

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
#define UID_MAPPED_வேர் ((uid_t) (INT32_MAX-1))
#define GID_MAPPED_வேர் ((gid_t) (INT32_MAX-1))

#define ETC_PASSWD_LOCK_FILENAME ".pwd.lock"
#define ETC_PASSWD_LOCK_PATH "/etc/" ETC_PASSWD_LOCK_FILENAME

/* The following macros add 1 when converting things, since UID 0 is a valid UID, while the pointer
 * NULL is special */
#define PTR_TO_UID(p) ((uid_t) (((uintptr_t) (p))-1))
#define UID_TO_PTR(u) ((வெறுமை*) (((uintptr_t) (u))+1))

#define PTR_TO_GID(p) ((gid_t) (((uintptr_t) (p))-1))
#define GID_TO_PTR(u) ((வெறுமை*) (((uintptr_t) (u))+1))

வகைவரை எண்ணிக்கை ValidUserFlags {
        VALID_USER_RELAX         = 1 << 0,
        VALID_USER_WARN          = 1 << 1,
        VALID_USER_ALLOW_NUMERIC = 1 << 2,
} ValidUserFlags;

bool valid_user_group_name(மாறிலி எழுத்து *u, ValidUserFlags flags);
bool valid_gecos(மாறிலி எழுத்து *d);
எழுத்து* mangle_gecos(மாறிலி எழுத்து *d);
bool valid_home(மாறிலி எழுத்து *p);
bool valid_shell(மாறிலி எழுத்து *p);

முழு maybe_setgroups(size_t size, மாறிலி gid_t *list);

bool synthesize_nobody(வெறுமை);

முழு fgetpwent_sane(FILE *stream, கட்டமைப்பு passwd **pw);
முழு fgetspent_sane(FILE *stream, கட்டமைப்பு spwd **sp);
முழு fgetgrent_sane(FILE *stream, கட்டமைப்பு group **gr);
முழு putpwent_sane(மாறிலி கட்டமைப்பு passwd *pw, FILE *stream);
முழு putspent_sane(மாறிலி கட்டமைப்பு spwd *sp, FILE *stream);
முழு putgrent_sane(மாறிலி கட்டமைப்பு group *gr, FILE *stream);
#எனில் ENABLE_GSHADOW
முழு fgetsgent_sane(FILE *stream, கட்டமைப்பு sgrp **sg);
முழு putsgent_sane(மாறிலி கட்டமைப்பு sgrp *sg, FILE *stream);
#endif

முழு is_this_me(மாறிலி எழுத்து *username);

மாறிலி எழுத்து* get_home_வேர்(வெறுமை);

நிலையான உள்வரி bool hashed_password_is_locked_or_invalid(மாறிலி எழுத்து *password) {
        திரும்பு password && password[0] != '$';
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

முழு sysconf_ngroups_max(வெறுமை);

முழு lookup_pwent_in_files(
                எழுத்து * மாறிலி *files,
                மாறிலி எழுத்து *name,
                uid_t uid,
                கட்டமைப்பு passwd **ret);
முழு lookup_grent_in_files(
                எழுத்து * மாறிலி *files,
                மாறிலி எழுத்து *name,
                gid_t gid,
                கட்டமைப்பு group **ret);

ssize_t lookup_groups_in_files(
                எழுத்து * மாறிலி *files,
                மாறிலி எழுத்து *name,
                gid_t gid,
                gid_t **ret);
முழு getgrouplist_malloc(மாறிலி எழுத்து *user, gid_t gid, gid_t **ret);
முழு initgroups_wrapper(மாறிலி எழுத்து *user, gid_t gid);

முழு getpwuid_malloc(uid_t uid, கட்டமைப்பு passwd **ret);
முழு getpwnam_malloc(மாறிலி எழுத்து *name, கட்டமைப்பு passwd **ret);

முழு getgrnam_malloc(மாறிலி எழுத்து *name, கட்டமைப்பு group **ret);
முழு getgrgid_malloc(gid_t gid, கட்டமைப்பு group **ret);
