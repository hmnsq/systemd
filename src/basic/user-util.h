/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <grp.h>
#اگر ENABLE_GSHADOW
#  include <gshadow.h>
#اگر_ختم
#شامل <pwd.h>
#شامل <shadow.h>

#شامل "forward.h"
#شامل "errno-util.h"

/* Users managed by systemd-homed. See https://systemd.io/UIDS-GIDS for details
 * how this range fits into the rest of the world. */
#تعریف_کرو HOME_UID_MIN ((uid_t) 60001)
#تعریف_کرو HOME_UID_MAX ((uid_t) 60513)

/* Users mapped from host into a container */
#تعریف_کرو MAP_UID_MIN ((uid_t) 60514)
#تعریف_کرو MAP_UID_MAX ((uid_t) 60577)

/* A helper to print an error message when user or group resolution fails. */
ثابت حرف* strerror_user(صحیح errnum, حرف *buf, size_t buflen);
#تعریف_کرو STRERROR_USER(errnum) strerror_user(errnum, (حرف[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)
ثابت حرف* strerror_گروہ(صحیح errnum, حرف *buf, size_t buflen);
#تعریف_کرو STRERROR_گروہ(errnum) strerror_گروہ(errnum, (حرف[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)

ساکن اندرونی bool ERRNO_IS_NEG_BAD_ACCOUNT(intmax_t r) {
        واپس اندر_سیٹ(r,
                      -ESRCH,
                      -ENOEXEC);
}
_DEFINE_ABS_WRAPPER(BAD_ACCOUNT);

bool uid_is_valid(uid_t uid) _const_;

ساکن اندرونی bool gid_is_valid(gid_t gid) {
        واپس uid_is_valid((uid_t) gid);
}

صحیح parse_uid(ثابت حرف *s, uid_t* ret_uid);
صحیح parse_uid_range(ثابت حرف *s, uid_t *ret_زیریں, uid_t *ret_بالائی);

ساکن اندرونی صحیح parse_gid(ثابت حرف *s, gid_t *ret_gid) {
        واپس parse_uid(s, (uid_t*) ret_gid);
}

حرف* getlogname_malloc(خالی);
حرف* getusername_malloc(خالی);

ثابت حرف* default_جڑ_shell_at(صحیح rfd);
ثابت حرف* default_جڑ_shell(ثابت حرف *جڑ);

bool is_nologin_shell(ثابت حرف *shell) خالص;
bool shell_is_placeholder(ثابت حرف *shell) خالص;

قسمتعریف شمار UserCredsFlags {
        USER_CREDS_PREFER_NSS           = 1 << 0,  /* if set, only synthesize user records if database lacks them. Normally we bypass the userdb entirely for the records we can synthesize */
        USER_CREDS_ALLOW_MISSING        = 1 << 1,  /* if a numeric UID string is resolved, be OK if there's no record for it */
        USER_CREDS_CLEAN                = 1 << 2,  /* try to clean up shell and home fields with invalid data */
        USER_CREDS_SUPPRESS_PLACEHOLDER = 1 << 3,  /* suppress home and/or shell fields if value is placeholder (root/empty/nologin) */
} UserCredsFlags;

صحیح get_user_creds(
                ثابت حرف *username,
                UserCredsFlags flags,
                حرف **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                حرف **ret_home,
                حرف **ret_shell);
صحیح get_گروہ_creds(ثابت حرف *groupname, UserCredsFlags flags, حرف **ret_name, gid_t *ret_gid);

حرف* uid_مقصد_name(uid_t uid);
حرف* gid_مقصد_name(gid_t gid);

صحیح اندر_gid(gid_t gid);
صحیح اندر_گروہ(ثابت حرف *name);

صحیح ملاؤ_gid_lists(ثابت gid_t *list1, size_t size1, ثابت gid_t *list2, size_t size2, gid_t **ret);
صحیح getgroups_alloc(gid_t **ret);

صحیح get_home_dir(حرف **ret);
صحیح get_shell(حرف **ret);

صحیح fully_سیٹ_uid_gid(uid_t uid, gid_t gid, ثابت gid_t supplementary_gids[], size_t n_supplementary_gids);
ساکن اندرونی صحیح reset_uid_gid(خالی) {
        واپس fully_سیٹ_uid_gid(0, 0, تہی, 0);
}

صحیح take_etc_passwd_تالا(ثابت حرف *جڑ);

#تعریف_کرو UID_INVALID ((uid_t) -1)
#تعریف_کرو GID_INVALID ((gid_t) -1)

#تعریف_کرو UID_NOBODY ((uid_t) 65534U)
#تعریف_کرو GID_NOBODY ((gid_t) 65534U)

/* Conventional size of a user-namespace UID/GID delegation block (64K).
 * Untyped so it can be used in both UID and GID contexts without casts. */
#تعریف_کرو USERNS_RANGE_سائز 0x10000U

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
#تعریف_کرو UID_MAPPED_جڑ ((uid_t) (INT32_MAX-1))
#تعریف_کرو GID_MAPPED_جڑ ((gid_t) (INT32_MAX-1))

#تعریف_کرو ETC_PASSWD_تالا_FILENAME ".pwd.lock"
#تعریف_کرو ETC_PASSWD_تالا_راستہ "/etc/" ETC_PASSWD_تالا_FILENAME

/* The following macros add 1 when converting things, since UID 0 is a valid UID, while the pointer
 * NULL is special */
#تعریف_کرو PTR_مقصد_UID(p) ((uid_t) (((uintptr_t) (p))-1))
#تعریف_کرو UID_مقصد_PTR(u) ((خالی*) (((uintptr_t) (u))+1))

#تعریف_کرو PTR_مقصد_GID(p) ((gid_t) (((uintptr_t) (p))-1))
#تعریف_کرو GID_مقصد_PTR(u) ((خالی*) (((uintptr_t) (u))+1))

قسمتعریف شمار ValidUserFlags {
        VALID_USER_RELAX         = 1 << 0,
        VALID_USER_WARN          = 1 << 1,
        VALID_USER_ALLOW_NUMERIC = 1 << 2,
} ValidUserFlags;

bool valid_user_گروہ_name(ثابت حرف *u, ValidUserFlags flags);
bool valid_gecos(ثابت حرف *d);
حرف* mangle_gecos(ثابت حرف *d);
bool valid_home(ثابت حرف *p);
bool valid_shell(ثابت حرف *p);

صحیح maybe_setgroups(size_t سائز, ثابت gid_t *list);

bool synthesize_nobody(خالی);

صحیح fgetpwent_sane(FILE *دھارا, ساخت passwd **pw);
صحیح fgetspent_sane(FILE *دھارا, ساخت spwd **sp);
صحیح fgetgrent_sane(FILE *دھارا, ساخت group **gr);
صحیح putpwent_sane(ثابت ساخت passwd *pw, FILE *دھارا);
صحیح putspent_sane(ثابت ساخت spwd *sp, FILE *دھارا);
صحیح putgrent_sane(ثابت ساخت group *gr, FILE *دھارا);
#اگر ENABLE_GSHADOW
صحیح fgetsgent_sane(FILE *دھارا, ساخت sgrp **sg);
صحیح putsgent_sane(ثابت ساخت sgrp *sg, FILE *دھارا);
#اگر_ختم

صحیح is_this_me(ثابت حرف *username);

ثابت حرف* get_home_جڑ(خالی);

ساکن اندرونی bool hashed_password_is_locked_or_invalid(ثابت حرف *password) {
        واپس password && password[0] != '$';
}

/* Places where we will try to load account data from */
#تعریف_کرو PASSWD_FILES STRV_MAKE("/etc/passwd", "/usr/lib/passwd")
#تعریف_کرو گروہ_FILES STRV_MAKE("/etc/group", "/usr/lib/group")

/* A locked *and* invalid password for "struct spwd"'s .sp_pwdp and "struct passwd"'s .pw_passwd field */
#تعریف_کرو PASSWORD_LOCKED_AND_INVALID "!*"

/* A password indicating "look in shadow file, please!" for "struct passwd"'s .pw_passwd */
#تعریف_کرو PASSWORD_SEE_SHADOW "x"

/* A password indicating "hey, no password required for login" */
#تعریف_کرو PASSWORD_NONE ""

/* Used by sysusers to indicate that the password should be filled in by firstboot.
 * Also see https://github.com/systemd/systemd/pull/24680#pullrequestreview-1439464325.
 */
#تعریف_کرو PASSWORD_UNPROVISIONED "!unprovisioned"

صحیح sysconf_ngroups_max(خالی);

صحیح lookup_pwent_اندر_files(
                حرف * ثابت *files,
                ثابت حرف *name,
                uid_t uid,
                ساخت passwd **ret);
صحیح lookup_grent_اندر_files(
                حرف * ثابت *files,
                ثابت حرف *name,
                gid_t gid,
                ساخت group **ret);

ssize_t lookup_groups_اندر_files(
                حرف * ثابت *files,
                ثابت حرف *name,
                gid_t gid,
                gid_t **ret);
صحیح getgrouplist_malloc(ثابت حرف *user, gid_t gid, gid_t **ret);
صحیح initgroups_wrapper(ثابت حرف *user, gid_t gid);

صحیح getpwuid_malloc(uid_t uid, ساخت passwd **ret);
صحیح getpwnam_malloc(ثابت حرف *name, ساخت passwd **ret);

صحیح getgrnam_malloc(ثابت حرف *name, ساخت group **ret);
صحیح getgrgid_malloc(gid_t gid, ساخت group **ret);
