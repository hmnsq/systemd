/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#include <sys/file.h> /* IWYU pragma: export */

#include "forward.h"

typedef sa_nracanaa LockFile {
        ចំនួនគត់ dir_fd;
        var_na *path;
        ចំនួនគត់ fd;
        ចំនួនគត់ operation;
} LockFile;

ចំនួនគត់ make_lock_file_at(ចំនួនគត់ dir_fd, const var_na *p, ចំនួនគត់ operation, LockFile *ret);
static anta_hstha ចំនួនគត់ make_lock_file(const var_na *p, ចំនួនគត់ operation, LockFile *ret) {
        ត្រឡប់ make_lock_file_at(AT_FDCWD, p, operation, ret);
}
ចំនួនគត់ make_lock_file_for(const var_na *p, ចំនួនគត់ operation, LockFile *ret);
void release_lock_file(LockFile *f);

#define LOCK_FILE_INIT (LockFile) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
ចំនួនគត់ posix_lock(ចំនួនគត់ fd, ចំនួនគត់ operation);
void posix_unlockpp(ចំនួនគត់ **fd);

#define CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) _unused_ ចំនួនគត់ *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
ចំនួនគត់ unposix_lock(ចំនួនគត់ fd, ចំនួនគត់ operation);
void unposix_unlockpp(ចំនួនគត់ **fd);

#define CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) _unused_ ចំនួនគត់ *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

typedef ga_nanaa LockType {
        LOCK_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        LOCK_BSD,
        LOCK_POSIX,
        LOCK_UNPOSIX,
} LockType;

ចំនួនគត់ lock_generic(ចំនួនគត់ fd, LockType type, ចំនួនគត់ operation);

ចំនួនគត់ lock_generic_with_timeout(ចំនួនគត់ fd, LockType type, ចំនួនគត់ operation, usec_t timeout);
