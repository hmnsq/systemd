/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <grp.h>
#ਜੇ ENABLE_GSHADOW
#  include <gshadow.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <pwd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <shadow.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"

/* Users managed by systemd-homed. See https://systemd.io/UIDS-GIDS for details
 * how this range fits into the rest of the world. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HOME_UID_MIN ((uid_t) 60001)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HOME_UID_MAX ((uid_t) 60513)

/* Users mapped from host into a container */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAP_UID_MIN ((uid_t) 60514)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAP_UID_MAX ((uid_t) 60577)

/* A helper to print an error message when user or group resolution fails. */
ਅਚਲ ਅੱਖਰ* strerror_user(ਪੂਰਨ errnum, ਅੱਖਰ *buf, size_t buflen);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRERROR_USER(errnum) strerror_user(errnum, (ਅੱਖਰ[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)
ਅਚਲ ਅੱਖਰ* strerror_ਸਮੂਹ(ਪੂਰਨ errnum, ਅੱਖਰ *buf, size_t buflen);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRERROR_ਸਮੂਹ(errnum) strerror_ਸਮੂਹ(errnum, (ਅੱਖਰ[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)

ਸਥਿਰ ਸਤਰਅੰਦਰ bool ERRNO_IS_NEG_BAD_ACCOUNT(intmax_t r) {
        ਵਾਪਸ ਵਿੱਚ_ਸੈੱਟ(r,
                      -ESRCH,
                      -ENOEXEC);
}
_DEFINE_ABS_WRAPPER(BAD_ACCOUNT);

bool uid_is_valid(uid_t uid) _const_;

ਸਥਿਰ ਸਤਰਅੰਦਰ bool gid_is_valid(gid_t gid) {
        ਵਾਪਸ uid_is_valid((uid_t) gid);
}

ਪੂਰਨ parse_uid(ਅਚਲ ਅੱਖਰ *s, uid_t* ret_uid);
ਪੂਰਨ parse_uid_range(ਅਚਲ ਅੱਖਰ *s, uid_t *ret_ਹੇਠਲਾ, uid_t *ret_ਉੱਤਲਾ);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ parse_gid(ਅਚਲ ਅੱਖਰ *s, gid_t *ret_gid) {
        ਵਾਪਸ parse_uid(s, (uid_t*) ret_gid);
}

ਅੱਖਰ* getlogname_malloc(ਖਾਲੀ);
ਅੱਖਰ* getusername_malloc(ਖਾਲੀ);

ਅਚਲ ਅੱਖਰ* default_ਜੜ੍ਹ_shell_at(ਪੂਰਨ rfd);
ਅਚਲ ਅੱਖਰ* default_ਜੜ੍ਹ_shell(ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ);

bool is_nologin_shell(ਅਚਲ ਅੱਖਰ *shell) ਸ਼ੁੱਧ;
bool shell_is_placeholder(ਅਚਲ ਅੱਖਰ *shell) ਸ਼ੁੱਧ;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ UserCredsFlags {
        USER_CREDS_PREFER_NSS           = 1 << 0,  /* if set, only synthesize user records if database lacks them. Normally we bypass the userdb entirely for the records we can synthesize */
        USER_CREDS_ALLOW_MISSING        = 1 << 1,  /* if a numeric UID string is resolved, be OK if there's no record for it */
        USER_CREDS_CLEAN                = 1 << 2,  /* try to clean up shell and home fields with invalid data */
        USER_CREDS_SUPPRESS_PLACEHOLDER = 1 << 3,  /* suppress home and/or shell fields if value is placeholder (root/empty/nologin) */
} UserCredsFlags;

ਪੂਰਨ get_user_creds(
                ਅਚਲ ਅੱਖਰ *username,
                UserCredsFlags flags,
                ਅੱਖਰ **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                ਅੱਖਰ **ret_home,
                ਅੱਖਰ **ret_shell);
ਪੂਰਨ get_ਸਮੂਹ_creds(ਅਚਲ ਅੱਖਰ *groupname, UserCredsFlags flags, ਅੱਖਰ **ret_name, gid_t *ret_gid);

ਅੱਖਰ* uid_ਮੰਜ਼ਿਲ_name(uid_t uid);
ਅੱਖਰ* gid_ਮੰਜ਼ਿਲ_name(gid_t gid);

ਪੂਰਨ ਵਿੱਚ_gid(gid_t gid);
ਪੂਰਨ ਵਿੱਚ_ਸਮੂਹ(ਅਚਲ ਅੱਖਰ *name);

ਪੂਰਨ ਮਿਲਾਓ_gid_lists(ਅਚਲ gid_t *list1, size_t size1, ਅਚਲ gid_t *list2, size_t size2, gid_t **ret);
ਪੂਰਨ getgroups_alloc(gid_t **ret);

ਪੂਰਨ get_home_dir(ਅੱਖਰ **ret);
ਪੂਰਨ get_shell(ਅੱਖਰ **ret);

ਪੂਰਨ fully_ਸੈੱਟ_uid_gid(uid_t uid, gid_t gid, ਅਚਲ gid_t supplementary_gids[], size_t n_supplementary_gids);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ reset_uid_gid(ਖਾਲੀ) {
        ਵਾਪਸ fully_ਸੈੱਟ_uid_gid(0, 0, ਨਿਰਾ, 0);
}

ਪੂਰਨ take_etc_passwd_ਤਾਲਾ(ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UID_INVALID ((uid_t) -1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GID_INVALID ((gid_t) -1)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UID_NOBODY ((uid_t) 65534U)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GID_NOBODY ((gid_t) 65534U)

/* Conventional size of a user-namespace UID/GID delegation block (64K).
 * Untyped so it can be used in both UID and GID contexts without casts. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USERNS_RANGE_ਮਾਪ 0x10000U

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UID_MAPPED_ਜੜ੍ਹ ((uid_t) (INT32_MAX-1))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GID_MAPPED_ਜੜ੍ਹ ((gid_t) (INT32_MAX-1))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ETC_PASSWD_ਤਾਲਾ_FILENAME ".pwd.lock"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ETC_PASSWD_ਤਾਲਾ_ਰਸਤਾ "/etc/" ETC_PASSWD_ਤਾਲਾ_FILENAME

/* The following macros add 1 when converting things, since UID 0 is a valid UID, while the pointer
 * NULL is special */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTR_ਮੰਜ਼ਿਲ_UID(p) ((uid_t) (((uintptr_t) (p))-1))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UID_ਮੰਜ਼ਿਲ_PTR(u) ((ਖਾਲੀ*) (((uintptr_t) (u))+1))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTR_ਮੰਜ਼ਿਲ_GID(p) ((gid_t) (((uintptr_t) (p))-1))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GID_ਮੰਜ਼ਿਲ_PTR(u) ((ਖਾਲੀ*) (((uintptr_t) (u))+1))

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ValidUserFlags {
        VALID_USER_RELAX         = 1 << 0,
        VALID_USER_WARN          = 1 << 1,
        VALID_USER_ALLOW_NUMERIC = 1 << 2,
} ValidUserFlags;

bool valid_user_ਸਮੂਹ_name(ਅਚਲ ਅੱਖਰ *u, ValidUserFlags flags);
bool valid_gecos(ਅਚਲ ਅੱਖਰ *d);
ਅੱਖਰ* mangle_gecos(ਅਚਲ ਅੱਖਰ *d);
bool valid_home(ਅਚਲ ਅੱਖਰ *p);
bool valid_shell(ਅਚਲ ਅੱਖਰ *p);

ਪੂਰਨ maybe_setgroups(size_t ਮਾਪ, ਅਚਲ gid_t *list);

bool synthesize_nobody(ਖਾਲੀ);

ਪੂਰਨ fgetpwent_sane(FILE *ਧਾਰਾ, ਬਣਤਰ passwd **pw);
ਪੂਰਨ fgetspent_sane(FILE *ਧਾਰਾ, ਬਣਤਰ spwd **sp);
ਪੂਰਨ fgetgrent_sane(FILE *ਧਾਰਾ, ਬਣਤਰ group **gr);
ਪੂਰਨ putpwent_sane(ਅਚਲ ਬਣਤਰ passwd *pw, FILE *ਧਾਰਾ);
ਪੂਰਨ putspent_sane(ਅਚਲ ਬਣਤਰ spwd *sp, FILE *ਧਾਰਾ);
ਪੂਰਨ putgrent_sane(ਅਚਲ ਬਣਤਰ group *gr, FILE *ਧਾਰਾ);
#ਜੇ ENABLE_GSHADOW
ਪੂਰਨ fgetsgent_sane(FILE *ਧਾਰਾ, ਬਣਤਰ sgrp **sg);
ਪੂਰਨ putsgent_sane(ਅਚਲ ਬਣਤਰ sgrp *sg, FILE *ਧਾਰਾ);
#ਜੇ_ਅੰਤ

ਪੂਰਨ is_this_me(ਅਚਲ ਅੱਖਰ *username);

ਅਚਲ ਅੱਖਰ* get_home_ਜੜ੍ਹ(ਖਾਲੀ);

ਸਥਿਰ ਸਤਰਅੰਦਰ bool hashed_password_is_locked_or_invalid(ਅਚਲ ਅੱਖਰ *password) {
        ਵਾਪਸ password && password[0] != '$';
}

/* Places where we will try to load account data from */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PASSWD_FILES STRV_MAKE("/etc/passwd", "/usr/lib/passwd")
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਮੂਹ_FILES STRV_MAKE("/etc/group", "/usr/lib/group")

/* A locked *and* invalid password for "struct spwd"'s .sp_pwdp and "struct passwd"'s .pw_passwd field */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PASSWORD_LOCKED_AND_INVALID "!*"

/* A password indicating "look in shadow file, please!" for "struct passwd"'s .pw_passwd */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PASSWORD_SEE_SHADOW "x"

/* A password indicating "hey, no password required for login" */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PASSWORD_NONE ""

/* Used by sysusers to indicate that the password should be filled in by firstboot.
 * Also see https://github.com/systemd/systemd/pull/24680#pullrequestreview-1439464325.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PASSWORD_UNPROVISIONED "!unprovisioned"

ਪੂਰਨ sysconf_ngroups_max(ਖਾਲੀ);

ਪੂਰਨ lookup_pwent_ਵਿੱਚ_files(
                ਅੱਖਰ * ਅਚਲ *files,
                ਅਚਲ ਅੱਖਰ *name,
                uid_t uid,
                ਬਣਤਰ passwd **ret);
ਪੂਰਨ lookup_grent_ਵਿੱਚ_files(
                ਅੱਖਰ * ਅਚਲ *files,
                ਅਚਲ ਅੱਖਰ *name,
                gid_t gid,
                ਬਣਤਰ group **ret);

ssize_t lookup_groups_ਵਿੱਚ_files(
                ਅੱਖਰ * ਅਚਲ *files,
                ਅਚਲ ਅੱਖਰ *name,
                gid_t gid,
                gid_t **ret);
ਪੂਰਨ getgrouplist_malloc(ਅਚਲ ਅੱਖਰ *user, gid_t gid, gid_t **ret);
ਪੂਰਨ initgroups_wrapper(ਅਚਲ ਅੱਖਰ *user, gid_t gid);

ਪੂਰਨ getpwuid_malloc(uid_t uid, ਬਣਤਰ passwd **ret);
ਪੂਰਨ getpwnam_malloc(ਅਚਲ ਅੱਖਰ *name, ਬਣਤਰ passwd **ret);

ਪੂਰਨ getgrnam_malloc(ਅਚਲ ਅੱਖਰ *name, ਬਣਤਰ group **ret);
ਪੂਰਨ getgrgid_malloc(gid_t gid, ਬਣਤਰ group **ret);
