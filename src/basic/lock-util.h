/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#include <sys/file.h> /* IWYU pragma: export */

#include "forward.h"

typedef struct LockFile {
        цео dir_fd;
        char *path;
        цео fd;
        цео operation;
} LockFile;

цео make_lock_file_at(цео dir_fd, const char *p, цео operation, LockFile *ret);
static inline цео make_lock_file(const char *p, цео operation, LockFile *ret) {
        врати make_lock_file_at(AT_FDCWD, p, operation, ret);
}
цео make_lock_file_for(const char *p, цео operation, LockFile *ret);
void release_lock_file(LockFile *f);

#define LOCK_FILE_INIT (LockFile) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
цео posix_lock(цео fd, цео operation);
void posix_unlockpp(цео **fd);

#define CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) _unused_ цео *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
цео unposix_lock(цео fd, цео operation);
void unposix_unlockpp(цео **fd);

#define CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) _unused_ цео *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

typedef enum LockType {
        LOCK_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        LOCK_BSD,
        LOCK_POSIX,
        LOCK_UNPOSIX,
} LockType;

цео lock_generic(цео fd, LockType type, цео operation);

цео lock_generic_with_timeout(цео fd, LockType type, цео operation, usec_t timeout);
