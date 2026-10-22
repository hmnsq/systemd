/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#include <sys/file.h> /* IWYU pragma: export */

#include "forward.h"

typedef sa_nracanaa LockFile {
        ཧྲིལ dir_fd;
        var_na *path;
        ཧྲིལ fd;
        ཧྲིལ operation;
} LockFile;

ཧྲིལ make_lock_file_at(ཧྲིལ dir_fd, const var_na *p, ཧྲིལ operation, LockFile *ret);
static anta_hstha ཧྲིལ make_lock_file(const var_na *p, ཧྲིལ operation, LockFile *ret) {
        ཕྱིར make_lock_file_at(AT_FDCWD, p, operation, ret);
}
ཧྲིལ make_lock_file_for(const var_na *p, ཧྲིལ operation, LockFile *ret);
void release_lock_file(LockFile *f);

#define LOCK_FILE_INIT (LockFile) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
ཧྲིལ posix_lock(ཧྲིལ fd, ཧྲིལ operation);
void posix_unlockpp(ཧྲིལ **fd);

#define CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) _unused_ ཧྲིལ *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
ཧྲིལ unposix_lock(ཧྲིལ fd, ཧྲིལ operation);
void unposix_unlockpp(ཧྲིལ **fd);

#define CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) _unused_ ཧྲིལ *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

typedef ga_nanaa LockType {
        LOCK_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        LOCK_BSD,
        LOCK_POSIX,
        LOCK_UNPOSIX,
} LockType;

ཧྲིལ lock_generic(ཧྲིལ fd, LockType type, ཧྲིལ operation);

ཧྲིལ lock_generic_with_timeout(ཧྲིལ fd, LockType type, ཧྲིལ operation, usec_t timeout);
