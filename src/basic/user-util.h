/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <grp.h>
#यदि ENABLE_GSHADOW
#  include <gshadow.h>
#यदि_अन्तः
#अन्तर्भाव <pwd.h>
#अन्तर्भाव <shadow.h>

#अन्तर्भाव "forward.h"
#अन्तर्भाव "errno-util.h"

/* Users managed by systemd-homed. See https://systemd.io/UIDS-GIDS for details
 * how this range fits into the rest of the world. */
#परिभाषय HOME_UID_MIN ((uid_t) 60001)
#परिभाषय HOME_UID_MAX ((uid_t) 60513)

/* Users mapped from host into a container */
#परिभाषय MAP_UID_MIN ((uid_t) 60514)
#परिभाषय MAP_UID_MAX ((uid_t) 60577)

/* A helper to print an error message when user or group resolution fails. */
ध्रुव वर्ण* strerror_user(पूर्णाङ्क errnum, वर्ण *buf, size_t buflen);
#परिभाषय STRERROR_USER(errnum) strerror_user(errnum, (वर्ण[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)
ध्रुव वर्ण* strerror_गणः(पूर्णाङ्क errnum, वर्ण *buf, size_t buflen);
#परिभाषय STRERROR_गणः(errnum) strerror_गणः(errnum, (वर्ण[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)

स्थिर अन्तःस्थ bool ERRNO_IS_NEG_BAD_ACCOUNT(intmax_t r) {
        प्रत्यागम अन्तर्_समुच्चयः(r,
                      -ESRCH,
                      -ENOEXEC);
}
_DEFINE_ABS_WRAPPER(BAD_ACCOUNT);

bool uid_is_valid(uid_t uid) _const_;

स्थिर अन्तःस्थ bool gid_is_valid(gid_t gid) {
        प्रत्यागम uid_is_valid((uid_t) gid);
}

पूर्णाङ्क parse_uid(ध्रुव वर्ण *s, uid_t* ret_uid);
पूर्णाङ्क parse_uid_range(ध्रुव वर्ण *s, uid_t *ret_अधस्तनम्, uid_t *ret_ऊर्ध्वस्थम्);

स्थिर अन्तःस्थ पूर्णाङ्क parse_gid(ध्रुव वर्ण *s, gid_t *ret_gid) {
        प्रत्यागम parse_uid(s, (uid_t*) ret_gid);
}

वर्ण* getlogname_malloc(शून्य);
वर्ण* getusername_malloc(शून्य);

ध्रुव वर्ण* default_मूलम्_shell_at(पूर्णाङ्क rfd);
ध्रुव वर्ण* default_मूलम्_shell(ध्रुव वर्ण *मूलम्);

bool is_nologin_shell(ध्रुव वर्ण *shell) विशुद्धम्;
bool shell_is_placeholder(ध्रुव वर्ण *shell) विशुद्धम्;

प्रकारनिर्देश गणना UserCredsFlags {
        USER_CREDS_PREFER_NSS           = 1 << 0,  /* if set, only synthesize user records if database lacks them. Normally we bypass the userdb entirely for the records we can synthesize */
        USER_CREDS_ALLOW_MISSING        = 1 << 1,  /* if a numeric UID string is resolved, be OK if there's no record for it */
        USER_CREDS_CLEAN                = 1 << 2,  /* try to clean up shell and home fields with invalid data */
        USER_CREDS_SUPPRESS_PLACEHOLDER = 1 << 3,  /* suppress home and/or shell fields if value is placeholder (root/empty/nologin) */
} UserCredsFlags;

पूर्णाङ्क get_user_creds(
                ध्रुव वर्ण *username,
                UserCredsFlags flags,
                वर्ण **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                वर्ण **ret_home,
                वर्ण **ret_shell);
पूर्णाङ्क get_गणः_creds(ध्रुव वर्ण *groupname, UserCredsFlags flags, वर्ण **ret_name, gid_t *ret_gid);

वर्ण* uid_गन्तव्यम्_name(uid_t uid);
वर्ण* gid_गन्तव्यम्_name(gid_t gid);

पूर्णाङ्क अन्तर्_gid(gid_t gid);
पूर्णाङ्क अन्तर्_गणः(ध्रुव वर्ण *name);

पूर्णाङ्क merge_gid_lists(ध्रुव gid_t *list1, size_t size1, ध्रुव gid_t *list2, size_t size2, gid_t **ret);
पूर्णाङ्क getgroups_alloc(gid_t **ret);

पूर्णाङ्क get_home_dir(वर्ण **ret);
पूर्णाङ्क get_shell(वर्ण **ret);

पूर्णाङ्क fully_समुच्चयः_uid_gid(uid_t uid, gid_t gid, ध्रुव gid_t supplementary_gids[], size_t n_supplementary_gids);
स्थिर अन्तःस्थ पूर्णाङ्क reset_uid_gid(शून्य) {
        प्रत्यागम fully_समुच्चयः_uid_gid(0, 0, रिक्त, 0);
}

पूर्णाङ्क take_etc_passwd_तालकम्(ध्रुव वर्ण *मूलम्);

#परिभाषय UID_INVALID ((uid_t) -1)
#परिभाषय GID_INVALID ((gid_t) -1)

#परिभाषय UID_NOBODY ((uid_t) 65534U)
#परिभाषय GID_NOBODY ((gid_t) 65534U)

/* Conventional size of a user-namespace UID/GID delegation block (64K).
 * Untyped so it can be used in both UID and GID contexts without casts. */
#परिभाषय USERNS_RANGE_आकार 0x10000U

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
#परिभाषय UID_MAPPED_मूलम् ((uid_t) (INT32_MAX-1))
#परिभाषय GID_MAPPED_मूलम् ((gid_t) (INT32_MAX-1))

#परिभाषय ETC_PASSWD_तालकम्_FILENAME ".pwd.lock"
#परिभाषय ETC_PASSWD_तालकम्_मार्गः "/etc/" ETC_PASSWD_तालकम्_FILENAME

/* The following macros add 1 when converting things, since UID 0 is a valid UID, while the pointer
 * NULL is special */
#परिभाषय PTR_गन्तव्यम्_UID(p) ((uid_t) (((uintptr_t) (p))-1))
#परिभाषय UID_गन्तव्यम्_PTR(u) ((शून्य*) (((uintptr_t) (u))+1))

#परिभाषय PTR_गन्तव्यम्_GID(p) ((gid_t) (((uintptr_t) (p))-1))
#परिभाषय GID_गन्तव्यम्_PTR(u) ((शून्य*) (((uintptr_t) (u))+1))

प्रकारनिर्देश गणना ValidUserFlags {
        VALID_USER_RELAX         = 1 << 0,
        VALID_USER_WARN          = 1 << 1,
        VALID_USER_ALLOW_NUMERIC = 1 << 2,
} ValidUserFlags;

bool valid_user_गणः_name(ध्रुव वर्ण *u, ValidUserFlags flags);
bool valid_gecos(ध्रुव वर्ण *d);
वर्ण* mangle_gecos(ध्रुव वर्ण *d);
bool valid_home(ध्रुव वर्ण *p);
bool valid_shell(ध्रुव वर्ण *p);

पूर्णाङ्क maybe_setgroups(size_t आकार, ध्रुव gid_t *list);

bool synthesize_nobody(शून्य);

पूर्णाङ्क fgetpwent_sane(FILE *प्रवाहः, संरचना passwd **pw);
पूर्णाङ्क fgetspent_sane(FILE *प्रवाहः, संरचना spwd **sp);
पूर्णाङ्क fgetgrent_sane(FILE *प्रवाहः, संरचना group **gr);
पूर्णाङ्क putpwent_sane(ध्रुव संरचना passwd *pw, FILE *प्रवाहः);
पूर्णाङ्क putspent_sane(ध्रुव संरचना spwd *sp, FILE *प्रवाहः);
पूर्णाङ्क putgrent_sane(ध्रुव संरचना group *gr, FILE *प्रवाहः);
#यदि ENABLE_GSHADOW
पूर्णाङ्क fgetsgent_sane(FILE *प्रवाहः, संरचना sgrp **sg);
पूर्णाङ्क putsgent_sane(ध्रुव संरचना sgrp *sg, FILE *प्रवाहः);
#यदि_अन्तः

पूर्णाङ्क is_this_me(ध्रुव वर्ण *username);

ध्रुव वर्ण* get_home_मूलम्(शून्य);

स्थिर अन्तःस्थ bool hashed_password_is_locked_or_invalid(ध्रुव वर्ण *password) {
        प्रत्यागम password && password[0] != '$';
}

/* Places where we will try to load account data from */
#परिभाषय PASSWD_FILES STRV_MAKE("/etc/passwd", "/usr/lib/passwd")
#परिभाषय गणः_FILES STRV_MAKE("/etc/group", "/usr/lib/group")

/* A locked *and* invalid password for "struct spwd"'s .sp_pwdp and "struct passwd"'s .pw_passwd field */
#परिभाषय PASSWORD_LOCKED_AND_INVALID "!*"

/* A password indicating "look in shadow file, please!" for "struct passwd"'s .pw_passwd */
#परिभाषय PASSWORD_SEE_SHADOW "x"

/* A password indicating "hey, no password required for login" */
#परिभाषय PASSWORD_NONE ""

/* Used by sysusers to indicate that the password should be filled in by firstboot.
 * Also see https://github.com/systemd/systemd/pull/24680#pullrequestreview-1439464325.
 */
#परिभाषय PASSWORD_UNPROVISIONED "!unprovisioned"

पूर्णाङ्क sysconf_ngroups_max(शून्य);

पूर्णाङ्क lookup_pwent_अन्तर्_files(
                वर्ण * ध्रुव *files,
                ध्रुव वर्ण *name,
                uid_t uid,
                संरचना passwd **ret);
पूर्णाङ्क lookup_grent_अन्तर्_files(
                वर्ण * ध्रुव *files,
                ध्रुव वर्ण *name,
                gid_t gid,
                संरचना group **ret);

ssize_t lookup_groups_अन्तर्_files(
                वर्ण * ध्रुव *files,
                ध्रुव वर्ण *name,
                gid_t gid,
                gid_t **ret);
पूर्णाङ्क getgrouplist_malloc(ध्रुव वर्ण *user, gid_t gid, gid_t **ret);
पूर्णाङ्क initgroups_wrapper(ध्रुव वर्ण *user, gid_t gid);

पूर्णाङ्क getpwuid_malloc(uid_t uid, संरचना passwd **ret);
पूर्णाङ्क getpwnam_malloc(ध्रुव वर्ण *name, संरचना passwd **ret);

पूर्णाङ्क getgrnam_malloc(ध्रुव वर्ण *name, संरचना group **ret);
पूर्णाङ्क getgrgid_malloc(gid_t gid, संरचना group **ret);
