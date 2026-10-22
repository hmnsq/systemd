/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن <grp.h>
#إذا ENABLE_GSHADOW
#  include <gshadow.h>
#پایان_اگر
#ضمن <pwd.h>
#ضمن <shadow.h>

#ضمن "forward.h"
#ضمن "errno-util.h"

/* Users managed by systemd-homed. See https://systemd.io/UIDS-GIDS for details
 * how this range fits into the rest of the world. */
#تعریف_کن HOME_UID_MIN ((uid_t) 60001)
#تعریف_کن HOME_UID_MAX ((uid_t) 60513)

/* Users mapped from host into a container */
#تعریف_کن MAP_UID_MIN ((uid_t) 60514)
#تعریف_کن MAP_UID_MAX ((uid_t) 60577)

/* A helper to print an error message when user or group resolution fails. */
ثابت حرف* strerror_user(صحيح errnum, حرف *buf, size_t buflen);
#تعریف_کن STRERROR_USER(errnum) strerror_user(errnum, (حرف[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)
ثابت حرف* strerror_گروه(صحيح errnum, حرف *buf, size_t buflen);
#تعریف_کن STRERROR_گروه(errnum) strerror_گروه(errnum, (حرف[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)

ساكن مضمن bool ERRNO_IS_NEG_BAD_ACCOUNT(intmax_t r) {
        أرجع درون_مجموعه(r,
                      -ESRCH,
                      -ENOEXEC);
}
_DEFINE_ABS_WRAPPER(BAD_ACCOUNT);

bool uid_is_valid(uid_t uid) _const_;

ساكن مضمن bool gid_is_valid(gid_t gid) {
        أرجع uid_is_valid((uid_t) gid);
}

صحيح parse_uid(ثابت حرف *s, uid_t* ret_uid);
صحيح parse_uid_range(ثابت حرف *s, uid_t *ret_پایینی, uid_t *ret_فوقانی);

ساكن مضمن صحيح parse_gid(ثابت حرف *s, gid_t *ret_gid) {
        أرجع parse_uid(s, (uid_t*) ret_gid);
}

حرف* getlogname_malloc(فراغ);
حرف* getusername_malloc(فراغ);

ثابت حرف* default_ریشه_shell_at(صحيح rfd);
ثابت حرف* default_ریشه_shell(ثابت حرف *ریشه);

bool is_nologin_shell(ثابت حرف *shell) خالص;
bool shell_is_placeholder(ثابت حرف *shell) خالص;

تعريفنوع تعداد UserCredsFlags {
        USER_CREDS_PREFER_NSS           = 1 << 0,  /* if set, only synthesize user records if database lacks them. Normally we bypass the userdb entirely for the records we can synthesize */
        USER_CREDS_ALLOW_MISSING        = 1 << 1,  /* if a numeric UID string is resolved, be OK if there's no record for it */
        USER_CREDS_CLEAN                = 1 << 2,  /* try to clean up shell and home fields with invalid data */
        USER_CREDS_SUPPRESS_PLACEHOLDER = 1 << 3,  /* suppress home and/or shell fields if value is placeholder (root/empty/nologin) */
} UserCredsFlags;

صحيح get_user_creds(
                ثابت حرف *username,
                UserCredsFlags flags,
                حرف **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                حرف **ret_home,
                حرف **ret_shell);
صحيح get_گروه_creds(ثابت حرف *groupname, UserCredsFlags flags, حرف **ret_name, gid_t *ret_gid);

حرف* uid_مقصد_name(uid_t uid);
حرف* gid_مقصد_name(gid_t gid);

صحيح درون_gid(gid_t gid);
صحيح درون_گروه(ثابت حرف *name);

صحيح ادغام_gid_lists(ثابت gid_t *list1, size_t size1, ثابت gid_t *list2, size_t size2, gid_t **ret);
صحيح getgroups_alloc(gid_t **ret);

صحيح get_home_dir(حرف **ret);
صحيح get_shell(حرف **ret);

صحيح fully_مجموعه_uid_gid(uid_t uid, gid_t gid, ثابت gid_t supplementary_gids[], size_t n_supplementary_gids);
ساكن مضمن صحيح reset_uid_gid(فراغ) {
        أرجع fully_مجموعه_uid_gid(0, 0, عدم, 0);
}

صحيح take_etc_passwd_قفل(ثابت حرف *ریشه);

#تعریف_کن UID_INVALID ((uid_t) -1)
#تعریف_کن GID_INVALID ((gid_t) -1)

#تعریف_کن UID_NOBODY ((uid_t) 65534U)
#تعریف_کن GID_NOBODY ((gid_t) 65534U)

/* Conventional size of a user-namespace UID/GID delegation block (64K).
 * Untyped so it can be used in both UID and GID contexts without casts. */
#تعریف_کن USERNS_RANGE_اندازه 0x10000U

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
#تعریف_کن UID_MAPPED_ریشه ((uid_t) (INT32_MAX-1))
#تعریف_کن GID_MAPPED_ریشه ((gid_t) (INT32_MAX-1))

#تعریف_کن ETC_PASSWD_قفل_FILENAME ".pwd.lock"
#تعریف_کن ETC_PASSWD_قفل_مسیر "/etc/" ETC_PASSWD_قفل_FILENAME

/* The following macros add 1 when converting things, since UID 0 is a valid UID, while the pointer
 * NULL is special */
#تعریف_کن PTR_مقصد_UID(p) ((uid_t) (((uintptr_t) (p))-1))
#تعریف_کن UID_مقصد_PTR(u) ((فراغ*) (((uintptr_t) (u))+1))

#تعریف_کن PTR_مقصد_GID(p) ((gid_t) (((uintptr_t) (p))-1))
#تعریف_کن GID_مقصد_PTR(u) ((فراغ*) (((uintptr_t) (u))+1))

تعريفنوع تعداد ValidUserFlags {
        VALID_USER_RELAX         = 1 << 0,
        VALID_USER_WARN          = 1 << 1,
        VALID_USER_ALLOW_NUMERIC = 1 << 2,
} ValidUserFlags;

bool valid_user_گروه_name(ثابت حرف *u, ValidUserFlags flags);
bool valid_gecos(ثابت حرف *d);
حرف* mangle_gecos(ثابت حرف *d);
bool valid_home(ثابت حرف *p);
bool valid_shell(ثابت حرف *p);

صحيح maybe_setgroups(size_t اندازه, ثابت gid_t *list);

bool synthesize_nobody(فراغ);

صحيح fgetpwent_sane(FILE *جریان, بنية passwd **pw);
صحيح fgetspent_sane(FILE *جریان, بنية spwd **sp);
صحيح fgetgrent_sane(FILE *جریان, بنية group **gr);
صحيح putpwent_sane(ثابت بنية passwd *pw, FILE *جریان);
صحيح putspent_sane(ثابت بنية spwd *sp, FILE *جریان);
صحيح putgrent_sane(ثابت بنية group *gr, FILE *جریان);
#إذا ENABLE_GSHADOW
صحيح fgetsgent_sane(FILE *جریان, بنية sgrp **sg);
صحيح putsgent_sane(ثابت بنية sgrp *sg, FILE *جریان);
#پایان_اگر

صحيح is_this_me(ثابت حرف *username);

ثابت حرف* get_home_ریشه(فراغ);

ساكن مضمن bool hashed_password_is_locked_or_invalid(ثابت حرف *password) {
        أرجع password && password[0] != '$';
}

/* Places where we will try to load account data from */
#تعریف_کن PASSWD_FILES STRV_MAKE("/etc/passwd", "/usr/lib/passwd")
#تعریف_کن گروه_FILES STRV_MAKE("/etc/group", "/usr/lib/group")

/* A locked *and* invalid password for "struct spwd"'s .sp_pwdp and "struct passwd"'s .pw_passwd field */
#تعریف_کن PASSWORD_LOCKED_AND_INVALID "!*"

/* A password indicating "look in shadow file, please!" for "struct passwd"'s .pw_passwd */
#تعریف_کن PASSWORD_SEE_SHADOW "x"

/* A password indicating "hey, no password required for login" */
#تعریف_کن PASSWORD_NONE ""

/* Used by sysusers to indicate that the password should be filled in by firstboot.
 * Also see https://github.com/systemd/systemd/pull/24680#pullrequestreview-1439464325.
 */
#تعریف_کن PASSWORD_UNPROVISIONED "!unprovisioned"

صحيح sysconf_ngroups_max(فراغ);

صحيح lookup_pwent_درون_files(
                حرف * ثابت *files,
                ثابت حرف *name,
                uid_t uid,
                بنية passwd **ret);
صحيح lookup_grent_درون_files(
                حرف * ثابت *files,
                ثابت حرف *name,
                gid_t gid,
                بنية group **ret);

ssize_t lookup_groups_درون_files(
                حرف * ثابت *files,
                ثابت حرف *name,
                gid_t gid,
                gid_t **ret);
صحيح getgrouplist_malloc(ثابت حرف *user, gid_t gid, gid_t **ret);
صحيح initgroups_wrapper(ثابت حرف *user, gid_t gid);

صحيح getpwuid_malloc(uid_t uid, بنية passwd **ret);
صحيح getpwnam_malloc(ثابت حرف *name, بنية passwd **ret);

صحيح getgrnam_malloc(ثابت حرف *name, بنية group **ret);
صحيح getgrgid_malloc(gid_t gid, بنية group **ret);
