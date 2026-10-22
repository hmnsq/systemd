/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن <grp.h>
#إذا ENABLE_GSHADOW
#  include <gshadow.h>
#endif
#ضمن <pwd.h>
#ضمن <shadow.h>

#ضمن "forward.h"
#ضمن "errno-util.h"

/* Users managed by systemd-homed. See https://systemd.io/UIDS-GIDS for details
 * how this range fits into the rest of the world. */
#define HOME_UID_MIN ((uid_t) 60001)
#define HOME_UID_MAX ((uid_t) 60513)

/* Users mapped from host into a container */
#define MAP_UID_MIN ((uid_t) 60514)
#define MAP_UID_MAX ((uid_t) 60577)

/* A helper to print an error message when user or group resolution fails. */
ثابت حرف* strerror_user(صحيح errnum, حرف *buf, size_t buflen);
#define STRERROR_USER(errnum) strerror_user(errnum, (حرف[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)
ثابت حرف* strerror_group(صحيح errnum, حرف *buf, size_t buflen);
#define STRERROR_GROUP(errnum) strerror_group(errnum, (حرف[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)

ساكن مضمن bool ERRNO_IS_NEG_BAD_ACCOUNT(intmax_t r) {
        إرجاع IN_SET(r,
                      -ESRCH,
                      -ENOEXEC);
}
_DEFINE_ABS_WRAPPER(BAD_ACCOUNT);

bool uid_is_valid(uid_t uid) _const_;

ساكن مضمن bool gid_is_valid(gid_t gid) {
        إرجاع uid_is_valid((uid_t) gid);
}

صحيح parse_uid(ثابت حرف *s, uid_t* ret_uid);
صحيح parse_uid_range(ثابت حرف *s, uid_t *ret_lower, uid_t *ret_upper);

ساكن مضمن صحيح parse_gid(ثابت حرف *s, gid_t *ret_gid) {
        إرجاع parse_uid(s, (uid_t*) ret_gid);
}

حرف* getlogname_malloc(فراغ);
حرف* getusername_malloc(فراغ);

ثابت حرف* default_جذر_shell_at(صحيح rfd);
ثابت حرف* default_جذر_shell(ثابت حرف *جذر);

bool is_nologin_shell(ثابت حرف *shell) _pure_;
bool shell_is_placeholder(ثابت حرف *shell) _pure_;

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
صحيح get_group_creds(ثابت حرف *groupname, UserCredsFlags flags, حرف **ret_name, gid_t *ret_gid);

حرف* uid_to_name(uid_t uid);
حرف* gid_to_name(gid_t gid);

صحيح in_gid(gid_t gid);
صحيح in_group(ثابت حرف *name);

صحيح merge_gid_lists(ثابت gid_t *list1, size_t size1, ثابت gid_t *list2, size_t size2, gid_t **ret);
صحيح getgroups_alloc(gid_t **ret);

صحيح get_home_dir(حرف **ret);
صحيح get_shell(حرف **ret);

صحيح fully_set_uid_gid(uid_t uid, gid_t gid, ثابت gid_t supplementary_gids[], size_t n_supplementary_gids);
ساكن مضمن صحيح reset_uid_gid(فراغ) {
        إرجاع fully_set_uid_gid(0, 0, عدم, 0);
}

صحيح take_etc_passwd_lock(ثابت حرف *جذر);

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
#define UID_MAPPED_جذر ((uid_t) (INT32_MAX-1))
#define GID_MAPPED_جذر ((gid_t) (INT32_MAX-1))

#define ETC_PASSWD_LOCK_FILENAME ".pwd.lock"
#define ETC_PASSWD_LOCK_PATH "/etc/" ETC_PASSWD_LOCK_FILENAME

/* The following macros add 1 when converting things, since UID 0 is a valid UID, while the pointer
 * NULL is special */
#define PTR_TO_UID(p) ((uid_t) (((uintptr_t) (p))-1))
#define UID_TO_PTR(u) ((فراغ*) (((uintptr_t) (u))+1))

#define PTR_TO_GID(p) ((gid_t) (((uintptr_t) (p))-1))
#define GID_TO_PTR(u) ((فراغ*) (((uintptr_t) (u))+1))

تعريفنوع تعداد ValidUserFlags {
        VALID_USER_RELAX         = 1 << 0,
        VALID_USER_WARN          = 1 << 1,
        VALID_USER_ALLOW_NUMERIC = 1 << 2,
} ValidUserFlags;

bool valid_user_group_name(ثابت حرف *u, ValidUserFlags flags);
bool valid_gecos(ثابت حرف *d);
حرف* mangle_gecos(ثابت حرف *d);
bool valid_home(ثابت حرف *p);
bool valid_shell(ثابت حرف *p);

صحيح maybe_setgroups(size_t size, ثابت gid_t *list);

bool synthesize_nobody(فراغ);

صحيح fgetpwent_sane(FILE *stream, بنية passwd **pw);
صحيح fgetspent_sane(FILE *stream, بنية spwd **sp);
صحيح fgetgrent_sane(FILE *stream, بنية group **gr);
صحيح putpwent_sane(ثابت بنية passwd *pw, FILE *stream);
صحيح putspent_sane(ثابت بنية spwd *sp, FILE *stream);
صحيح putgrent_sane(ثابت بنية group *gr, FILE *stream);
#إذا ENABLE_GSHADOW
صحيح fgetsgent_sane(FILE *stream, بنية sgrp **sg);
صحيح putsgent_sane(ثابت بنية sgrp *sg, FILE *stream);
#endif

صحيح is_this_me(ثابت حرف *username);

ثابت حرف* get_home_جذر(فراغ);

ساكن مضمن bool hashed_password_is_locked_or_invalid(ثابت حرف *password) {
        إرجاع password && password[0] != '$';
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

صحيح sysconf_ngroups_max(فراغ);

صحيح lookup_pwent_in_files(
                حرف * ثابت *files,
                ثابت حرف *name,
                uid_t uid,
                بنية passwd **ret);
صحيح lookup_grent_in_files(
                حرف * ثابت *files,
                ثابت حرف *name,
                gid_t gid,
                بنية group **ret);

ssize_t lookup_groups_in_files(
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
