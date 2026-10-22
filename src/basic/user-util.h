/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <grp.h>
#ଯଦି ENABLE_GSHADOW
#  include <gshadow.h>
#endif
#ଅନ୍ତର୍ଭାଵ <pwd.h>
#ଅନ୍ତର୍ଭାଵ <shadow.h>

#ଅନ୍ତର୍ଭାଵ "forward.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"

/* Users managed by systemd-homed. See https://systemd.io/UIDS-GIDS for details
 * how this range fits into the rest of the world. */
#define HOME_UID_MIN ((uid_t) 60001)
#define HOME_UID_MAX ((uid_t) 60513)

/* Users mapped from host into a container */
#define MAP_UID_MIN ((uid_t) 60514)
#define MAP_UID_MAX ((uid_t) 60577)

/* A helper to print an error message when user or group resolution fails. */
ଅଚଳ ଅକ୍ଷର* strerror_user(ପୂର୍ଣ୍ଣସଂଖ୍ୟା errnum, ଅକ୍ଷର *buf, size_t buflen);
#define STRERROR_USER(errnum) strerror_user(errnum, (ଅକ୍ଷର[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)
ଅଚଳ ଅକ୍ଷର* strerror_group(ପୂର୍ଣ୍ଣସଂଖ୍ୟା errnum, ଅକ୍ଷର *buf, size_t buflen);
#define STRERROR_GROUP(errnum) strerror_group(errnum, (ଅକ୍ଷର[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ERRNO_IS_NEG_BAD_ACCOUNT(intmax_t r) {
        ଫେରନ୍ତୁ IN_SET(r,
                      -ESRCH,
                      -ENOEXEC);
}
_DEFINE_ABS_WRAPPER(BAD_ACCOUNT);

bool uid_is_valid(uid_t uid) _const_;

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool gid_is_valid(gid_t gid) {
        ଫେରନ୍ତୁ uid_is_valid((uid_t) gid);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_uid(ଅଚଳ ଅକ୍ଷର *s, uid_t* ret_uid);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_uid_range(ଅଚଳ ଅକ୍ଷର *s, uid_t *ret_lower, uid_t *ret_upper);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_gid(ଅଚଳ ଅକ୍ଷର *s, gid_t *ret_gid) {
        ଫେରନ୍ତୁ parse_uid(s, (uid_t*) ret_gid);
}

ଅକ୍ଷର* getlogname_malloc(ଶୂନ୍ୟ);
ଅକ୍ଷର* getusername_malloc(ଶୂନ୍ୟ);

ଅଚଳ ଅକ୍ଷର* default_ମୂଳ_shell_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd);
ଅଚଳ ଅକ୍ଷର* default_ମୂଳ_shell(ଅଚଳ ଅକ୍ଷର *ମୂଳ);

bool is_nologin_shell(ଅଚଳ ଅକ୍ଷର *shell) _pure_;
bool shell_is_placeholder(ଅଚଳ ଅକ୍ଷର *shell) _pure_;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା UserCredsFlags {
        USER_CREDS_PREFER_NSS           = 1 << 0,  /* if set, only synthesize user records if database lacks them. Normally we bypass the userdb entirely for the records we can synthesize */
        USER_CREDS_ALLOW_MISSING        = 1 << 1,  /* if a numeric UID string is resolved, be OK if there's no record for it */
        USER_CREDS_CLEAN                = 1 << 2,  /* try to clean up shell and home fields with invalid data */
        USER_CREDS_SUPPRESS_PLACEHOLDER = 1 << 3,  /* suppress home and/or shell fields if value is placeholder (root/empty/nologin) */
} UserCredsFlags;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_user_creds(
                ଅଚଳ ଅକ୍ଷର *username,
                UserCredsFlags flags,
                ଅକ୍ଷର **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                ଅକ୍ଷର **ret_home,
                ଅକ୍ଷର **ret_shell);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_group_creds(ଅଚଳ ଅକ୍ଷର *groupname, UserCredsFlags flags, ଅକ୍ଷର **ret_name, gid_t *ret_gid);

ଅକ୍ଷର* uid_to_name(uid_t uid);
ଅକ୍ଷର* gid_to_name(gid_t gid);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_gid(gid_t gid);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_group(ଅଚଳ ଅକ୍ଷର *name);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା merge_gid_lists(ଅଚଳ gid_t *list1, size_t size1, ଅଚଳ gid_t *list2, size_t size2, gid_t **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା getgroups_alloc(gid_t **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_home_dir(ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_shell(ଅକ୍ଷର **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fully_set_uid_gid(uid_t uid, gid_t gid, ଅଚଳ gid_t supplementary_gids[], size_t n_supplementary_gids);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା reset_uid_gid(ଶୂନ୍ୟ) {
        ଫେରନ୍ତୁ fully_set_uid_gid(0, 0, ଖାଲି, 0);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା take_etc_passwd_lock(ଅଚଳ ଅକ୍ଷର *ମୂଳ);

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
#define UID_MAPPED_ମୂଳ ((uid_t) (INT32_MAX-1))
#define GID_MAPPED_ମୂଳ ((gid_t) (INT32_MAX-1))

#define ETC_PASSWD_LOCK_FILENAME ".pwd.lock"
#define ETC_PASSWD_LOCK_PATH "/etc/" ETC_PASSWD_LOCK_FILENAME

/* The following macros add 1 when converting things, since UID 0 is a valid UID, while the pointer
 * NULL is special */
#define PTR_TO_UID(p) ((uid_t) (((uintptr_t) (p))-1))
#define UID_TO_PTR(u) ((ଶୂନ୍ୟ*) (((uintptr_t) (u))+1))

#define PTR_TO_GID(p) ((gid_t) (((uintptr_t) (p))-1))
#define GID_TO_PTR(u) ((ଶୂନ୍ୟ*) (((uintptr_t) (u))+1))

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା ValidUserFlags {
        VALID_USER_RELAX         = 1 << 0,
        VALID_USER_WARN          = 1 << 1,
        VALID_USER_ALLOW_NUMERIC = 1 << 2,
} ValidUserFlags;

bool valid_user_group_name(ଅଚଳ ଅକ୍ଷର *u, ValidUserFlags flags);
bool valid_gecos(ଅଚଳ ଅକ୍ଷର *d);
ଅକ୍ଷର* mangle_gecos(ଅଚଳ ଅକ୍ଷର *d);
bool valid_home(ଅଚଳ ଅକ୍ଷର *p);
bool valid_shell(ଅଚଳ ଅକ୍ଷର *p);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା maybe_setgroups(size_t size, ଅଚଳ gid_t *list);

bool synthesize_nobody(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fgetpwent_sane(FILE *stream, ସଂରଚନା passwd **pw);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fgetspent_sane(FILE *stream, ସଂରଚନା spwd **sp);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fgetgrent_sane(FILE *stream, ସଂରଚନା group **gr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା putpwent_sane(ଅଚଳ ସଂରଚନା passwd *pw, FILE *stream);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା putspent_sane(ଅଚଳ ସଂରଚନା spwd *sp, FILE *stream);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା putgrent_sane(ଅଚଳ ସଂରଚନା group *gr, FILE *stream);
#ଯଦି ENABLE_GSHADOW
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fgetsgent_sane(FILE *stream, ସଂରଚନା sgrp **sg);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା putsgent_sane(ଅଚଳ ସଂରଚନା sgrp *sg, FILE *stream);
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_this_me(ଅଚଳ ଅକ୍ଷର *username);

ଅଚଳ ଅକ୍ଷର* get_home_ମୂଳ(ଶୂନ୍ୟ);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool hashed_password_is_locked_or_invalid(ଅଚଳ ଅକ୍ଷର *password) {
        ଫେରନ୍ତୁ password && password[0] != '$';
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

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysconf_ngroups_max(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା lookup_pwent_in_files(
                ଅକ୍ଷର * ଅଚଳ *files,
                ଅଚଳ ଅକ୍ଷର *name,
                uid_t uid,
                ସଂରଚନା passwd **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା lookup_grent_in_files(
                ଅକ୍ଷର * ଅଚଳ *files,
                ଅଚଳ ଅକ୍ଷର *name,
                gid_t gid,
                ସଂରଚନା group **ret);

ssize_t lookup_groups_in_files(
                ଅକ୍ଷର * ଅଚଳ *files,
                ଅଚଳ ଅକ୍ଷର *name,
                gid_t gid,
                gid_t **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା getgrouplist_malloc(ଅଚଳ ଅକ୍ଷର *user, gid_t gid, gid_t **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା initgroups_wrapper(ଅଚଳ ଅକ୍ଷର *user, gid_t gid);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getpwuid_malloc(uid_t uid, ସଂରଚନା passwd **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା getpwnam_malloc(ଅଚଳ ଅକ୍ଷର *name, ସଂରଚନା passwd **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getgrnam_malloc(ଅଚଳ ଅକ୍ଷର *name, ସଂରଚନା group **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା getgrgid_malloc(gid_t gid, ସଂରଚନା group **ret);
