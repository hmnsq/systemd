/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <grp.h>
#ಯದಿ ENABLE_GSHADOW
#  include <gshadow.h>
#endif
#ಅನ್ತರ್ಭಾವ <pwd.h>
#ಅನ್ತರ್ಭಾವ <shadow.h>

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"

/* Users managed by systemd-homed. See https://systemd.io/UIDS-GIDS for details
 * how this range fits into the rest of the world. */
#define HOME_UID_MIN ((uid_t) 60001)
#define HOME_UID_MAX ((uid_t) 60513)

/* Users mapped from host into a container */
#define MAP_UID_MIN ((uid_t) 60514)
#define MAP_UID_MAX ((uid_t) 60577)

/* A helper to print an error message when user or group resolution fails. */
ಧ್ರುವ ವರ್ಣ* strerror_user(ಪೂರ್ಣಾಙ್ಕ errnum, ವರ್ಣ *buf, size_t buflen);
#define STRERROR_USER(errnum) strerror_user(errnum, (ವರ್ಣ[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)
ಧ್ರುವ ವರ್ಣ* strerror_group(ಪೂರ್ಣಾಙ್ಕ errnum, ವರ್ಣ *buf, size_t buflen);
#define STRERROR_GROUP(errnum) strerror_group(errnum, (ವರ್ಣ[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool ERRNO_IS_NEG_BAD_ACCOUNT(intmax_t r) {
        ಪ್ರತ್ಯಾಗಮ IN_SET(r,
                      -ESRCH,
                      -ENOEXEC);
}
_DEFINE_ABS_WRAPPER(BAD_ACCOUNT);

bool uid_is_valid(uid_t uid) _const_;

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool gid_is_valid(gid_t gid) {
        ಪ್ರತ್ಯಾಗಮ uid_is_valid((uid_t) gid);
}

ಪೂರ್ಣಾಙ್ಕ parse_uid(ಧ್ರುವ ವರ್ಣ *s, uid_t* ret_uid);
ಪೂರ್ಣಾಙ್ಕ parse_uid_range(ಧ್ರುವ ವರ್ಣ *s, uid_t *ret_lower, uid_t *ret_upper);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ parse_gid(ಧ್ರುವ ವರ್ಣ *s, gid_t *ret_gid) {
        ಪ್ರತ್ಯಾಗಮ parse_uid(s, (uid_t*) ret_gid);
}

ವರ್ಣ* getlogname_malloc(ಶೂನ್ಯ);
ವರ್ಣ* getusername_malloc(ಶೂನ್ಯ);

ಧ್ರುವ ವರ್ಣ* default_ಬೇರು_shell_at(ಪೂರ್ಣಾಙ್ಕ rfd);
ಧ್ರುವ ವರ್ಣ* default_ಬೇರು_shell(ಧ್ರುವ ವರ್ಣ *ಬೇರು);

bool is_nologin_shell(ಧ್ರುವ ವರ್ಣ *shell) _pure_;
bool shell_is_placeholder(ಧ್ರುವ ವರ್ಣ *shell) _pure_;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ UserCredsFlags {
        USER_CREDS_PREFER_NSS           = 1 << 0,  /* if set, only synthesize user records if database lacks them. Normally we bypass the userdb entirely for the records we can synthesize */
        USER_CREDS_ALLOW_MISSING        = 1 << 1,  /* if a numeric UID string is resolved, be OK if there's no record for it */
        USER_CREDS_CLEAN                = 1 << 2,  /* try to clean up shell and home fields with invalid data */
        USER_CREDS_SUPPRESS_PLACEHOLDER = 1 << 3,  /* suppress home and/or shell fields if value is placeholder (root/empty/nologin) */
} UserCredsFlags;

ಪೂರ್ಣಾಙ್ಕ get_user_creds(
                ಧ್ರುವ ವರ್ಣ *username,
                UserCredsFlags flags,
                ವರ್ಣ **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                ವರ್ಣ **ret_home,
                ವರ್ಣ **ret_shell);
ಪೂರ್ಣಾಙ್ಕ get_group_creds(ಧ್ರುವ ವರ್ಣ *groupname, UserCredsFlags flags, ವರ್ಣ **ret_name, gid_t *ret_gid);

ವರ್ಣ* uid_to_name(uid_t uid);
ವರ್ಣ* gid_to_name(gid_t gid);

ಪೂರ್ಣಾಙ್ಕ in_gid(gid_t gid);
ಪೂರ್ಣಾಙ್ಕ in_group(ಧ್ರುವ ವರ್ಣ *name);

ಪೂರ್ಣಾಙ್ಕ merge_gid_lists(ಧ್ರುವ gid_t *list1, size_t size1, ಧ್ರುವ gid_t *list2, size_t size2, gid_t **ret);
ಪೂರ್ಣಾಙ್ಕ getgroups_alloc(gid_t **ret);

ಪೂರ್ಣಾಙ್ಕ get_home_dir(ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ get_shell(ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ fully_set_uid_gid(uid_t uid, gid_t gid, ಧ್ರುವ gid_t supplementary_gids[], size_t n_supplementary_gids);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ reset_uid_gid(ಶೂನ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ fully_set_uid_gid(0, 0, ರಿಕ್ತ, 0);
}

ಪೂರ್ಣಾಙ್ಕ take_etc_passwd_lock(ಧ್ರುವ ವರ್ಣ *ಬೇರು);

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
#define UID_MAPPED_ಬೇರು ((uid_t) (INT32_MAX-1))
#define GID_MAPPED_ಬೇರು ((gid_t) (INT32_MAX-1))

#define ETC_PASSWD_LOCK_FILENAME ".pwd.lock"
#define ETC_PASSWD_LOCK_PATH "/etc/" ETC_PASSWD_LOCK_FILENAME

/* The following macros add 1 when converting things, since UID 0 is a valid UID, while the pointer
 * NULL is special */
#define PTR_TO_UID(p) ((uid_t) (((uintptr_t) (p))-1))
#define UID_TO_PTR(u) ((ಶೂನ್ಯ*) (((uintptr_t) (u))+1))

#define PTR_TO_GID(p) ((gid_t) (((uintptr_t) (p))-1))
#define GID_TO_PTR(u) ((ಶೂನ್ಯ*) (((uintptr_t) (u))+1))

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ ValidUserFlags {
        VALID_USER_RELAX         = 1 << 0,
        VALID_USER_WARN          = 1 << 1,
        VALID_USER_ALLOW_NUMERIC = 1 << 2,
} ValidUserFlags;

bool valid_user_group_name(ಧ್ರುವ ವರ್ಣ *u, ValidUserFlags flags);
bool valid_gecos(ಧ್ರುವ ವರ್ಣ *d);
ವರ್ಣ* mangle_gecos(ಧ್ರುವ ವರ್ಣ *d);
bool valid_home(ಧ್ರುವ ವರ್ಣ *p);
bool valid_shell(ಧ್ರುವ ವರ್ಣ *p);

ಪೂರ್ಣಾಙ್ಕ maybe_setgroups(size_t size, ಧ್ರುವ gid_t *list);

bool synthesize_nobody(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ fgetpwent_sane(FILE *stream, ಸಂರಚನಾ passwd **pw);
ಪೂರ್ಣಾಙ್ಕ fgetspent_sane(FILE *stream, ಸಂರಚನಾ spwd **sp);
ಪೂರ್ಣಾಙ್ಕ fgetgrent_sane(FILE *stream, ಸಂರಚನಾ group **gr);
ಪೂರ್ಣಾಙ್ಕ putpwent_sane(ಧ್ರುವ ಸಂರಚನಾ passwd *pw, FILE *stream);
ಪೂರ್ಣಾಙ್ಕ putspent_sane(ಧ್ರುವ ಸಂರಚನಾ spwd *sp, FILE *stream);
ಪೂರ್ಣಾಙ್ಕ putgrent_sane(ಧ್ರುವ ಸಂರಚನಾ group *gr, FILE *stream);
#ಯದಿ ENABLE_GSHADOW
ಪೂರ್ಣಾಙ್ಕ fgetsgent_sane(FILE *stream, ಸಂರಚನಾ sgrp **sg);
ಪೂರ್ಣಾಙ್ಕ putsgent_sane(ಧ್ರುವ ಸಂರಚನಾ sgrp *sg, FILE *stream);
#endif

ಪೂರ್ಣಾಙ್ಕ is_this_me(ಧ್ರುವ ವರ್ಣ *username);

ಧ್ರುವ ವರ್ಣ* get_home_ಬೇರು(ಶೂನ್ಯ);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool hashed_password_is_locked_or_invalid(ಧ್ರುವ ವರ್ಣ *password) {
        ಪ್ರತ್ಯಾಗಮ password && password[0] != '$';
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

ಪೂರ್ಣಾಙ್ಕ sysconf_ngroups_max(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ lookup_pwent_in_files(
                ವರ್ಣ * ಧ್ರುವ *files,
                ಧ್ರುವ ವರ್ಣ *name,
                uid_t uid,
                ಸಂರಚನಾ passwd **ret);
ಪೂರ್ಣಾಙ್ಕ lookup_grent_in_files(
                ವರ್ಣ * ಧ್ರುವ *files,
                ಧ್ರುವ ವರ್ಣ *name,
                gid_t gid,
                ಸಂರಚನಾ group **ret);

ssize_t lookup_groups_in_files(
                ವರ್ಣ * ಧ್ರುವ *files,
                ಧ್ರುವ ವರ್ಣ *name,
                gid_t gid,
                gid_t **ret);
ಪೂರ್ಣಾಙ್ಕ getgrouplist_malloc(ಧ್ರುವ ವರ್ಣ *user, gid_t gid, gid_t **ret);
ಪೂರ್ಣಾಙ್ಕ initgroups_wrapper(ಧ್ರುವ ವರ್ಣ *user, gid_t gid);

ಪೂರ್ಣಾಙ್ಕ getpwuid_malloc(uid_t uid, ಸಂರಚನಾ passwd **ret);
ಪೂರ್ಣಾಙ್ಕ getpwnam_malloc(ಧ್ರುವ ವರ್ಣ *name, ಸಂರಚನಾ passwd **ret);

ಪೂರ್ಣಾಙ್ಕ getgrnam_malloc(ಧ್ರುವ ವರ್ಣ *name, ಸಂರಚನಾ group **ret);
ಪೂರ್ಣಾಙ್ಕ getgrgid_malloc(gid_t gid, ಸಂರಚನಾ group **ret);
