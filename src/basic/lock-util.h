/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#include <sys/file.h> /* IWYU pragma: export */

#include "forward.h"

typedef struct LockFile {
        정수 dir_fd;
        char *path;
        정수 fd;
        정수 operation;
} LockFile;

정수 make_lock_file_at(정수 dir_fd, const char *p, 정수 operation, LockFile *ret);
static inline 정수 make_lock_file(const char *p, 정수 operation, LockFile *ret) {
        반환 make_lock_file_at(AT_FDCWD, p, operation, ret);
}
정수 make_lock_file_for(const char *p, 정수 operation, LockFile *ret);
void release_lock_file(LockFile *f);

#define LOCK_FILE_INIT (LockFile) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
정수 posix_lock(정수 fd, 정수 operation);
void posix_unlockpp(정수 **fd);

#define CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) _unused_ 정수 *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
정수 unposix_lock(정수 fd, 정수 operation);
void unposix_unlockpp(정수 **fd);

#define CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) _unused_ 정수 *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

typedef enum LockType {
        LOCK_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        LOCK_BSD,
        LOCK_POSIX,
        LOCK_UNPOSIX,
} LockType;

정수 lock_generic(정수 fd, LockType type, 정수 operation);

정수 lock_generic_with_timeout(정수 fd, LockType type, 정수 operation, usec_t timeout);
