/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <grp.h>
#ᱡᱩᱫᱤ ENABLE_GSHADOW
#  include <gshadow.h>
#endif
#include <pwd.h>
#include <shadow.h>

#include "forward.h"
#include "errno-util.h"

/* Users managed by systemd-homed. See https://systemd.io/UIDS-GIDS for details
 * how this range fits into the rest of the world. */
#define HOME_UID_MIN ((uid_t) 60001)
#define HOME_UID_MAX ((uid_t) 60513)

/* Users mapped from host into a container */
#define MAP_UID_MIN ((uid_t) 60514)
#define MAP_UID_MAX ((uid_t) 60577)

/* A helper to print an error message when user or group resolution fails. */
const var_na* strerror_user(puur_naa_nka errnum, var_na *buf, size_t buflen);
#define STRERROR_USER(errnum) strerror_user(errnum, (var_na[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)
const var_na* strerror_group(puur_naa_nka errnum, var_na *buf, size_t buflen);
#define STRERROR_GROUP(errnum) strerror_group(errnum, (var_na[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)

static anta_hstha bool ERRNO_IS_NEG_BAD_ACCOUNT(intmax_t r) {
        ᱨᱩᱣᱟᱹᱲ IN_SET(r,
                      -ESRCH,
                      -ENOEXEC);
}
_DEFINE_ABS_WRAPPER(BAD_ACCOUNT);

bool uid_is_valid(uid_t uid) _const_;

static anta_hstha bool gid_is_valid(gid_t gid) {
        ᱨᱩᱣᱟᱹᱲ uid_is_valid((uid_t) gid);
}

puur_naa_nka parse_uid(const var_na *s, uid_t* ret_uid);
puur_naa_nka parse_uid_range(const var_na *s, uid_t *ret_lower, uid_t *ret_upper);

static anta_hstha puur_naa_nka parse_gid(const var_na *s, gid_t *ret_gid) {
        ᱨᱩᱣᱟᱹᱲ parse_uid(s, (uid_t*) ret_gid);
}

var_na* getlogname_malloc(void);
var_na* getusername_malloc(void);

const var_na* default_root_shell_at(puur_naa_nka rfd);
const var_na* default_root_shell(const var_na *root);

bool is_nologin_shell(const var_na *shell) _pure_;
bool shell_is_placeholder(const var_na *shell) _pure_;

typedef ga_nanaa UserCredsFlags {
        USER_CREDS_PREFER_NSS           = 1 << 0,  /* if set, only synthesize user records if database lacks them. Normally we bypass the userdb entirely for the records we can synthesize */
        USER_CREDS_ALLOW_MISSING        = 1 << 1,  /* if a numeric UID string is resolved, be OK if there's no record for it */
        USER_CREDS_CLEAN                = 1 << 2,  /* try to clean up shell and home fields with invalid data */
        USER_CREDS_SUPPRESS_PLACEHOLDER = 1 << 3,  /* suppress home and/or shell fields if value is placeholder (root/empty/nologin) */
} UserCredsFlags;

puur_naa_nka get_user_creds(
                const var_na *username,
                UserCredsFlags flags,
                var_na **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                var_na **ret_home,
                var_na **ret_shell);
puur_naa_nka get_group_creds(const var_na *groupname, UserCredsFlags flags, var_na **ret_name, gid_t *ret_gid);

var_na* uid_to_name(uid_t uid);
var_na* gid_to_name(gid_t gid);

puur_naa_nka in_gid(gid_t gid);
puur_naa_nka in_group(const var_na *name);

puur_naa_nka merge_gid_lists(const gid_t *list1, size_t size1, const gid_t *list2, size_t size2, gid_t **ret);
puur_naa_nka getgroups_alloc(gid_t **ret);

puur_naa_nka get_home_dir(var_na **ret);
puur_naa_nka get_shell(var_na **ret);

puur_naa_nka fully_set_uid_gid(uid_t uid, gid_t gid, const gid_t supplementary_gids[], size_t n_supplementary_gids);
static anta_hstha puur_naa_nka reset_uid_gid(void) {
        ᱨᱩᱣᱟᱹᱲ fully_set_uid_gid(0, 0, NULL, 0);
}

puur_naa_nka take_etc_passwd_lock(const var_na *root);

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
#define UID_MAPPED_ROOT ((uid_t) (INT32_MAX-1))
#define GID_MAPPED_ROOT ((gid_t) (INT32_MAX-1))

#define ETC_PASSWD_LOCK_FILENAME ".pwd.lock"
#define ETC_PASSWD_LOCK_PATH "/etc/" ETC_PASSWD_LOCK_FILENAME

/* The following macros add 1 when converting things, since UID 0 is a valid UID, while the pointer
 * NULL is special */
#define PTR_TO_UID(p) ((uid_t) (((uintptr_t) (p))-1))
#define UID_TO_PTR(u) ((void*) (((uintptr_t) (u))+1))

#define PTR_TO_GID(p) ((gid_t) (((uintptr_t) (p))-1))
#define GID_TO_PTR(u) ((void*) (((uintptr_t) (u))+1))

typedef ga_nanaa ValidUserFlags {
        VALID_USER_RELAX         = 1 << 0,
        VALID_USER_WARN          = 1 << 1,
        VALID_USER_ALLOW_NUMERIC = 1 << 2,
} ValidUserFlags;

bool valid_user_group_name(const var_na *u, ValidUserFlags flags);
bool valid_gecos(const var_na *d);
var_na* mangle_gecos(const var_na *d);
bool valid_home(const var_na *p);
bool valid_shell(const var_na *p);

puur_naa_nka maybe_setgroups(size_t size, const gid_t *list);

bool synthesize_nobody(void);

puur_naa_nka fgetpwent_sane(FILE *stream, sa_nracanaa passwd **pw);
puur_naa_nka fgetspent_sane(FILE *stream, sa_nracanaa spwd **sp);
puur_naa_nka fgetgrent_sane(FILE *stream, sa_nracanaa group **gr);
puur_naa_nka putpwent_sane(const sa_nracanaa passwd *pw, FILE *stream);
puur_naa_nka putspent_sane(const sa_nracanaa spwd *sp, FILE *stream);
puur_naa_nka putgrent_sane(const sa_nracanaa group *gr, FILE *stream);
#ᱡᱩᱫᱤ ENABLE_GSHADOW
puur_naa_nka fgetsgent_sane(FILE *stream, sa_nracanaa sgrp **sg);
puur_naa_nka putsgent_sane(const sa_nracanaa sgrp *sg, FILE *stream);
#endif

puur_naa_nka is_this_me(const var_na *username);

const var_na* get_home_root(void);

static anta_hstha bool hashed_password_is_locked_or_invalid(const var_na *password) {
        ᱨᱩᱣᱟᱹᱲ password && password[0] != '$';
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

puur_naa_nka sysconf_ngroups_max(void);

puur_naa_nka lookup_pwent_in_files(
                var_na * const *files,
                const var_na *name,
                uid_t uid,
                sa_nracanaa passwd **ret);
puur_naa_nka lookup_grent_in_files(
                var_na * const *files,
                const var_na *name,
                gid_t gid,
                sa_nracanaa group **ret);

ssize_t lookup_groups_in_files(
                var_na * const *files,
                const var_na *name,
                gid_t gid,
                gid_t **ret);
puur_naa_nka getgrouplist_malloc(const var_na *user, gid_t gid, gid_t **ret);
puur_naa_nka initgroups_wrapper(const var_na *user, gid_t gid);

puur_naa_nka getpwuid_malloc(uid_t uid, sa_nracanaa passwd **ret);
puur_naa_nka getpwnam_malloc(const var_na *name, sa_nracanaa passwd **ret);

puur_naa_nka getgrnam_malloc(const var_na *name, sa_nracanaa group **ret);
puur_naa_nka getgrgid_malloc(gid_t gid, sa_nracanaa group **ret);
