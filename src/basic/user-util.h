/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <grp.h>
#ಆದರೆ ENABLE_GSHADOW
#  include <gshadow.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <pwd.h>
#ಅನ್ತರ್ಭಾವ <shadow.h>

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"

/* Users managed by systemd-homed. See https://systemd.io/UIDS-GIDS for details
 * how this range fits into the rest of the world. */
#ವ್ಯಾಖ್ಯಾನಿಸು HOME_UID_MIN ((uid_t) 60001)
#ವ್ಯಾಖ್ಯಾನಿಸು HOME_UID_MAX ((uid_t) 60513)

/* Users mapped from host into a container */
#ವ್ಯಾಖ್ಯಾನಿಸು MAP_UID_MIN ((uid_t) 60514)
#ವ್ಯಾಖ್ಯಾನಿಸು MAP_UID_MAX ((uid_t) 60577)

/* A helper to print an error message when user or group resolution fails. */
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* strerror_user(ಪೂರ್ಣಾಂಕ errnum, ಅಕ್ಷರ *buf, size_t buflen);
#ವ್ಯಾಖ್ಯಾನಿಸು STRERROR_USER(errnum) strerror_user(errnum, (ಅಕ್ಷರ[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* strerror_ಗುಂಪು(ಪೂರ್ಣಾಂಕ errnum, ಅಕ್ಷರ *buf, size_t buflen);
#ವ್ಯಾಖ್ಯಾನಿಸು STRERROR_ಗುಂಪು(errnum) strerror_ಗುಂಪು(errnum, (ಅಕ್ಷರ[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ERRNO_IS_NEG_BAD_ACCOUNT(intmax_t r) {
        ಹಿಂತಿರುಗಿ ಒಳ_ಗಣ(r,
                      -ESRCH,
                      -ENOEXEC);
}
_DEFINE_ABS_WRAPPER(BAD_ACCOUNT);

bool uid_is_valid(uid_t uid) _const_;

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool gid_is_valid(gid_t gid) {
        ಹಿಂತಿರುಗಿ uid_is_valid((uid_t) gid);
}

ಪೂರ್ಣಾಂಕ parse_uid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, uid_t* ret_uid);
ಪೂರ್ಣಾಂಕ parse_uid_range(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, uid_t *ret_ಅಧೋ, uid_t *ret_ಊರ್ಧ್ವ);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ parse_gid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, gid_t *ret_gid) {
        ಹಿಂತಿರುಗಿ parse_uid(s, (uid_t*) ret_gid);
}

ಅಕ್ಷರ* getlogname_malloc(ಶೂನ್ಯ);
ಅಕ್ಷರ* getusername_malloc(ಶೂನ್ಯ);

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* default_ಬೇರು_shell_at(ಪೂರ್ಣಾಂಕ rfd);
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* default_ಬೇರು_shell(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು);

bool is_nologin_shell(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *shell) ಶುದ್ಧ;
bool shell_is_placeholder(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *shell) ಶುದ್ಧ;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ UserCredsFlags {
        USER_CREDS_PREFER_NSS           = 1 << 0,  /* if set, only synthesize user records if database lacks them. Normally we bypass the userdb entirely for the records we can synthesize */
        USER_CREDS_ALLOW_MISSING        = 1 << 1,  /* if a numeric UID string is resolved, be OK if there's no record for it */
        USER_CREDS_CLEAN                = 1 << 2,  /* try to clean up shell and home fields with invalid data */
        USER_CREDS_SUPPRESS_PLACEHOLDER = 1 << 3,  /* suppress home and/or shell fields if value is placeholder (root/empty/nologin) */
} UserCredsFlags;

ಪೂರ್ಣಾಂಕ get_user_creds(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *username,
                UserCredsFlags flags,
                ಅಕ್ಷರ **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                ಅಕ್ಷರ **ret_home,
                ಅಕ್ಷರ **ret_shell);
ಪೂರ್ಣಾಂಕ get_ಗುಂಪು_creds(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *groupname, UserCredsFlags flags, ಅಕ್ಷರ **ret_name, gid_t *ret_gid);

ಅಕ್ಷರ* uid_ಗಮ್ಯ_name(uid_t uid);
ಅಕ್ಷರ* gid_ಗಮ್ಯ_name(gid_t gid);

ಪೂರ್ಣಾಂಕ ಒಳ_gid(gid_t gid);
ಪೂರ್ಣಾಂಕ ಒಳ_ಗುಂಪು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);

ಪೂರ್ಣಾಂಕ ವಿಲೀನ_gid_lists(ಸ್ಥಿರಾಂಕ gid_t *list1, size_t size1, ಸ್ಥಿರಾಂಕ gid_t *list2, size_t size2, gid_t **ret);
ಪೂರ್ಣಾಂಕ getgroups_alloc(gid_t **ret);

ಪೂರ್ಣಾಂಕ get_home_dir(ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ get_shell(ಅಕ್ಷರ **ret);

ಪೂರ್ಣಾಂಕ fully_ಗಣ_uid_gid(uid_t uid, gid_t gid, ಸ್ಥಿರಾಂಕ gid_t supplementary_gids[], size_t n_supplementary_gids);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ reset_uid_gid(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ fully_ಗಣ_uid_gid(0, 0, ಖಾಲಿ, 0);
}

ಪೂರ್ಣಾಂಕ take_etc_passwd_ಬೀಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು);

#ವ್ಯಾಖ್ಯಾನಿಸು UID_INVALID ((uid_t) -1)
#ವ್ಯಾಖ್ಯಾನಿಸು GID_INVALID ((gid_t) -1)

#ವ್ಯಾಖ್ಯಾನಿಸು UID_NOBODY ((uid_t) 65534U)
#ವ್ಯಾಖ್ಯಾನಿಸು GID_NOBODY ((gid_t) 65534U)

/* Conventional size of a user-namespace UID/GID delegation block (64K).
 * Untyped so it can be used in both UID and GID contexts without casts. */
#ವ್ಯಾಖ್ಯಾನಿಸು USERNS_RANGE_ಗಾತ್ರ 0x10000U

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
#ವ್ಯಾಖ್ಯಾನಿಸು UID_MAPPED_ಬೇರು ((uid_t) (INT32_MAX-1))
#ವ್ಯಾಖ್ಯಾನಿಸು GID_MAPPED_ಬೇರು ((gid_t) (INT32_MAX-1))

#ವ್ಯಾಖ್ಯಾನಿಸು ETC_PASSWD_ಬೀಗ_FILENAME ".pwd.lock"
#ವ್ಯಾಖ್ಯಾನಿಸು ETC_PASSWD_ಬೀಗ_ಮಾರ್ಗ "/etc/" ETC_PASSWD_ಬೀಗ_FILENAME

/* The following macros add 1 when converting things, since UID 0 is a valid UID, while the pointer
 * NULL is special */
#ವ್ಯಾಖ್ಯಾನಿಸು PTR_ಗಮ್ಯ_UID(p) ((uid_t) (((uintptr_t) (p))-1))
#ವ್ಯಾಖ್ಯಾನಿಸು UID_ಗಮ್ಯ_PTR(u) ((ಶೂನ್ಯ*) (((uintptr_t) (u))+1))

#ವ್ಯಾಖ್ಯಾನಿಸು PTR_ಗಮ್ಯ_GID(p) ((gid_t) (((uintptr_t) (p))-1))
#ವ್ಯಾಖ್ಯಾನಿಸು GID_ಗಮ್ಯ_PTR(u) ((ಶೂನ್ಯ*) (((uintptr_t) (u))+1))

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ValidUserFlags {
        VALID_USER_RELAX         = 1 << 0,
        VALID_USER_WARN          = 1 << 1,
        VALID_USER_ALLOW_NUMERIC = 1 << 2,
} ValidUserFlags;

bool valid_user_ಗುಂಪು_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *u, ValidUserFlags flags);
bool valid_gecos(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *d);
ಅಕ್ಷರ* mangle_gecos(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *d);
bool valid_home(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p);
bool valid_shell(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p);

ಪೂರ್ಣಾಂಕ maybe_setgroups(size_t ಗಾತ್ರ_2, ಸ್ಥಿರಾಂಕ gid_t *list);

bool synthesize_nobody(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ fgetpwent_sane(FILE *ಹರಿವು, ರಚನೆ passwd **pw);
ಪೂರ್ಣಾಂಕ fgetspent_sane(FILE *ಹರಿವು, ರಚನೆ spwd **sp);
ಪೂರ್ಣಾಂಕ fgetgrent_sane(FILE *ಹರಿವು, ರಚನೆ group **gr);
ಪೂರ್ಣಾಂಕ putpwent_sane(ಸ್ಥಿರಾಂಕ ರಚನೆ passwd *pw, FILE *ಹರಿವು);
ಪೂರ್ಣಾಂಕ putspent_sane(ಸ್ಥಿರಾಂಕ ರಚನೆ spwd *sp, FILE *ಹರಿವು);
ಪೂರ್ಣಾಂಕ putgrent_sane(ಸ್ಥಿರಾಂಕ ರಚನೆ group *gr, FILE *ಹರಿವು);
#ಆದರೆ ENABLE_GSHADOW
ಪೂರ್ಣಾಂಕ fgetsgent_sane(FILE *ಹರಿವು, ರಚನೆ sgrp **sg);
ಪೂರ್ಣಾಂಕ putsgent_sane(ಸ್ಥಿರಾಂಕ ರಚನೆ sgrp *sg, FILE *ಹರಿವು);
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ is_this_me(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *username);

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* get_home_ಬೇರು(ಶೂನ್ಯ);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool hashed_password_is_locked_or_invalid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *password) {
        ಹಿಂತಿರುಗಿ password && password[0] != '$';
}

/* Places where we will try to load account data from */
#ವ್ಯಾಖ್ಯಾನಿಸು PASSWD_FILES STRV_MAKE("/etc/passwd", "/usr/lib/passwd")
#ವ್ಯಾಖ್ಯಾನಿಸು ಗುಂಪು_FILES STRV_MAKE("/etc/group", "/usr/lib/group")

/* A locked *and* invalid password for "struct spwd"'s .sp_pwdp and "struct passwd"'s .pw_passwd field */
#ವ್ಯಾಖ್ಯಾನಿಸು PASSWORD_LOCKED_AND_INVALID "!*"

/* A password indicating "look in shadow file, please!" for "struct passwd"'s .pw_passwd */
#ವ್ಯಾಖ್ಯಾನಿಸು PASSWORD_SEE_SHADOW "x"

/* A password indicating "hey, no password required for login" */
#ವ್ಯಾಖ್ಯಾನಿಸು PASSWORD_NONE ""

/* Used by sysusers to indicate that the password should be filled in by firstboot.
 * Also see https://github.com/systemd/systemd/pull/24680#pullrequestreview-1439464325.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು PASSWORD_UNPROVISIONED "!unprovisioned"

ಪೂರ್ಣಾಂಕ sysconf_ngroups_max(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ lookup_pwent_ಒಳ_files(
                ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *files,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                uid_t uid,
                ರಚನೆ passwd **ret);
ಪೂರ್ಣಾಂಕ lookup_grent_ಒಳ_files(
                ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *files,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                gid_t gid,
                ರಚನೆ group **ret);

ssize_t lookup_groups_ಒಳ_files(
                ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *files,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                gid_t gid,
                gid_t **ret);
ಪೂರ್ಣಾಂಕ getgrouplist_malloc(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user, gid_t gid, gid_t **ret);
ಪೂರ್ಣಾಂಕ initgroups_wrapper(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user, gid_t gid);

ಪೂರ್ಣಾಂಕ getpwuid_malloc(uid_t uid, ರಚನೆ passwd **ret);
ಪೂರ್ಣಾಂಕ getpwnam_malloc(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ರಚನೆ passwd **ret);

ಪೂರ್ಣಾಂಕ getgrnam_malloc(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ರಚನೆ group **ret);
ಪೂರ್ಣಾಂಕ getgrgid_malloc(gid_t gid, ರಚನೆ group **ret);
