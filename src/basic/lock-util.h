/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#include <sys/file.h> /* IWYU pragma: export */

#include "forward.h"

typedef sa_nracanaa LockFile {
        ကိန်းပြည့် dir_fd;
        var_na *path;
        ကိန်းပြည့် fd;
        ကိန်းပြည့် operation;
} LockFile;

ကိန်းပြည့် make_lock_file_at(ကိန်းပြည့် dir_fd, const var_na *p, ကိန်းပြည့် operation, LockFile *ret);
static anta_hstha ကိန်းပြည့် make_lock_file(const var_na *p, ကိန်းပြည့် operation, LockFile *ret) {
        ပြန်ပေး make_lock_file_at(AT_FDCWD, p, operation, ret);
}
ကိန်းပြည့် make_lock_file_for(const var_na *p, ကိန်းပြည့် operation, LockFile *ret);
void release_lock_file(LockFile *f);

#define LOCK_FILE_INIT (LockFile) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
ကိန်းပြည့် posix_lock(ကိန်းပြည့် fd, ကိန်းပြည့် operation);
void posix_unlockpp(ကိန်းပြည့် **fd);

#define CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) _unused_ ကိန်းပြည့် *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
ကိန်းပြည့် unposix_lock(ကိန်းပြည့် fd, ကိန်းပြည့် operation);
void unposix_unlockpp(ကိန်းပြည့် **fd);

#define CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) _unused_ ကိန်းပြည့် *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

typedef ga_nanaa LockType {
        LOCK_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        LOCK_BSD,
        LOCK_POSIX,
        LOCK_UNPOSIX,
} LockType;

ကိန်းပြည့် lock_generic(ကိန်းပြည့် fd, LockType type, ကိန်းပြည့် operation);

ကိန်းပြည့် lock_generic_with_timeout(ကိန်းပြည့် fd, LockType type, ကိန်းပြည့် operation, usec_t timeout);
