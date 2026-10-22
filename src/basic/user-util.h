/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <grp.h>
#जर ENABLE_GSHADOW
#  include <gshadow.h>
#endif
#अन्तर्भाव <pwd.h>
#अन्तर्भाव <shadow.h>

#अन्तर्भाव "forward.h"
#अन्तर्भाव "errno-util.h"

/* Users managed by systemd-homed. See https://systemd.io/UIDS-GIDS for details
 * how this range fits into the rest of the world. */
#define HOME_UID_MIN ((uid_t) 60001)
#define HOME_UID_MAX ((uid_t) 60513)

/* Users mapped from host into a container */
#define MAP_UID_MIN ((uid_t) 60514)
#define MAP_UID_MAX ((uid_t) 60577)

/* A helper to print an error message when user or group resolution fails. */
स्थिरांक अक्षर* strerror_user(पूर्णांक errnum, अक्षर *buf, माप_प्रकार buflen);
#define STRERROR_USER(errnum) strerror_user(errnum, (अक्षर[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)
स्थिरांक अक्षर* strerror_group(पूर्णांक errnum, अक्षर *buf, माप_प्रकार buflen);
#define STRERROR_GROUP(errnum) strerror_group(errnum, (अक्षर[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)

स्थिर ओळीत bool ERRNO_IS_NEG_BAD_ACCOUNT(intmax_t r) {
        परत IN_SET(r,
                      -ESRCH,
                      -ENOEXEC);
}
_DEFINE_ABS_WRAPPER(BAD_ACCOUNT);

bool uid_is_valid(uid_t uid) _const_;

स्थिर ओळीत bool gid_is_valid(gid_t gid) {
        परत uid_is_valid((uid_t) gid);
}

पूर्णांक parse_uid(स्थिरांक अक्षर *s, uid_t* ret_uid);
पूर्णांक parse_uid_range(स्थिरांक अक्षर *s, uid_t *ret_lower, uid_t *ret_upper);

स्थिर ओळीत पूर्णांक parse_gid(स्थिरांक अक्षर *s, gid_t *ret_gid) {
        परत parse_uid(s, (uid_t*) ret_gid);
}

अक्षर* getlogname_malloc(शून्य);
अक्षर* getusername_malloc(शून्य);

स्थिरांक अक्षर* default_मूळ_shell_at(पूर्णांक rfd);
स्थिरांक अक्षर* default_मूळ_shell(स्थिरांक अक्षर *मूळ);

bool is_nologin_shell(स्थिरांक अक्षर *shell) _pure_;
bool shell_is_placeholder(स्थिरांक अक्षर *shell) _pure_;

प्रकारव्याख्या गणना UserCredsFlags {
        USER_CREDS_PREFER_NSS           = 1 << 0,  /* if set, only synthesize user records if database lacks them. Normally we bypass the userdb entirely for the records we can synthesize */
        USER_CREDS_ALLOW_MISSING        = 1 << 1,  /* if a numeric UID string is resolved, be OK if there's no record for it */
        USER_CREDS_CLEAN                = 1 << 2,  /* try to clean up shell and home fields with invalid data */
        USER_CREDS_SUPPRESS_PLACEHOLDER = 1 << 3,  /* suppress home and/or shell fields if value is placeholder (root/empty/nologin) */
} UserCredsFlags;

पूर्णांक get_user_creds(
                स्थिरांक अक्षर *username,
                UserCredsFlags flags,
                अक्षर **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                अक्षर **ret_home,
                अक्षर **ret_shell);
पूर्णांक get_group_creds(स्थिरांक अक्षर *groupname, UserCredsFlags flags, अक्षर **ret_name, gid_t *ret_gid);

अक्षर* uid_to_name(uid_t uid);
अक्षर* gid_to_name(gid_t gid);

पूर्णांक in_gid(gid_t gid);
पूर्णांक in_group(स्थिरांक अक्षर *name);

पूर्णांक merge_gid_lists(स्थिरांक gid_t *list1, माप_प्रकार size1, स्थिरांक gid_t *list2, माप_प्रकार size2, gid_t **ret);
पूर्णांक getgroups_alloc(gid_t **ret);

पूर्णांक get_home_dir(अक्षर **ret);
पूर्णांक get_shell(अक्षर **ret);

पूर्णांक fully_set_uid_gid(uid_t uid, gid_t gid, स्थिरांक gid_t supplementary_gids[], माप_प्रकार n_supplementary_gids);
स्थिर ओळीत पूर्णांक reset_uid_gid(शून्य) {
        परत fully_set_uid_gid(0, 0, रिक्त, 0);
}

पूर्णांक take_etc_passwd_lock(स्थिरांक अक्षर *मूळ);

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
#define UID_MAPPED_मूळ ((uid_t) (INT32_MAX-1))
#define GID_MAPPED_मूळ ((gid_t) (INT32_MAX-1))

#define ETC_PASSWD_LOCK_FILENAME ".pwd.lock"
#define ETC_PASSWD_LOCK_PATH "/etc/" ETC_PASSWD_LOCK_FILENAME

/* The following macros add 1 when converting things, since UID 0 is a valid UID, while the pointer
 * NULL is special */
#define PTR_TO_UID(p) ((uid_t) (((uintptr_t) (p))-1))
#define UID_TO_PTR(u) ((शून्य*) (((uintptr_t) (u))+1))

#define PTR_TO_GID(p) ((gid_t) (((uintptr_t) (p))-1))
#define GID_TO_PTR(u) ((शून्य*) (((uintptr_t) (u))+1))

प्रकारव्याख्या गणना ValidUserFlags {
        VALID_USER_RELAX         = 1 << 0,
        VALID_USER_WARN          = 1 << 1,
        VALID_USER_ALLOW_NUMERIC = 1 << 2,
} ValidUserFlags;

bool valid_user_group_name(स्थिरांक अक्षर *u, ValidUserFlags flags);
bool valid_gecos(स्थिरांक अक्षर *d);
अक्षर* mangle_gecos(स्थिरांक अक्षर *d);
bool valid_home(स्थिरांक अक्षर *p);
bool valid_shell(स्थिरांक अक्षर *p);

पूर्णांक maybe_setgroups(माप_प्रकार size, स्थिरांक gid_t *list);

bool synthesize_nobody(शून्य);

पूर्णांक fgetpwent_sane(खाता *stream, संरचना passwd **pw);
पूर्णांक fgetspent_sane(खाता *stream, संरचना spwd **sp);
पूर्णांक fgetgrent_sane(खाता *stream, संरचना group **gr);
पूर्णांक putpwent_sane(स्थिरांक संरचना passwd *pw, खाता *stream);
पूर्णांक putspent_sane(स्थिरांक संरचना spwd *sp, खाता *stream);
पूर्णांक putgrent_sane(स्थिरांक संरचना group *gr, खाता *stream);
#जर ENABLE_GSHADOW
पूर्णांक fgetsgent_sane(खाता *stream, संरचना sgrp **sg);
पूर्णांक putsgent_sane(स्थिरांक संरचना sgrp *sg, खाता *stream);
#endif

पूर्णांक is_this_me(स्थिरांक अक्षर *username);

स्थिरांक अक्षर* get_home_मूळ(शून्य);

स्थिर ओळीत bool hashed_password_is_locked_or_invalid(स्थिरांक अक्षर *password) {
        परत password && password[0] != '$';
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

पूर्णांक sysconf_ngroups_max(शून्य);

पूर्णांक lookup_pwent_in_files(
                अक्षर * स्थिरांक *files,
                स्थिरांक अक्षर *name,
                uid_t uid,
                संरचना passwd **ret);
पूर्णांक lookup_grent_in_files(
                अक्षर * स्थिरांक *files,
                स्थिरांक अक्षर *name,
                gid_t gid,
                संरचना group **ret);

ssize_t lookup_groups_in_files(
                अक्षर * स्थिरांक *files,
                स्थिरांक अक्षर *name,
                gid_t gid,
                gid_t **ret);
पूर्णांक getgrouplist_malloc(स्थिरांक अक्षर *user, gid_t gid, gid_t **ret);
पूर्णांक initgroups_wrapper(स्थिरांक अक्षर *user, gid_t gid);

पूर्णांक getpwuid_malloc(uid_t uid, संरचना passwd **ret);
पूर्णांक getpwnam_malloc(स्थिरांक अक्षर *name, संरचना passwd **ret);

पूर्णांक getgrnam_malloc(स्थिरांक अक्षर *name, संरचना group **ret);
पूर्णांक getgrgid_malloc(gid_t gid, संरचना group **ret);
