/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#include <sys/file.h> /* IWYU pragma: export */

#include "forward.h"

typedef struct LockFile {
        ціле dir_fd;
        char *path;
        ціле fd;
        ціле operation;
} LockFile;

ціле make_lock_file_at(ціле dir_fd, const char *p, ціле operation, LockFile *ret);
static inline ціле make_lock_file(const char *p, ціле operation, LockFile *ret) {
        повернути make_lock_file_at(AT_FDCWD, p, operation, ret);
}
ціле make_lock_file_for(const char *p, ціле operation, LockFile *ret);
void release_lock_file(LockFile *f);

#define LOCK_FILE_INIT (LockFile) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
ціле posix_lock(ціле fd, ціле operation);
void posix_unlockpp(ціле **fd);

#define CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) _unused_ ціле *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
ціле unposix_lock(ціле fd, ціле operation);
void unposix_unlockpp(ціле **fd);

#define CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) _unused_ ціле *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

typedef enum LockType {
        LOCK_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        LOCK_BSD,
        LOCK_POSIX,
        LOCK_UNPOSIX,
} LockType;

ціле lock_generic(ціле fd, LockType type, ціле operation);

ціле lock_generic_with_timeout(ціле fd, LockType type, ціле operation, usec_t timeout);
