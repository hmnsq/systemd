/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#include <sys/file.h> /* IWYU pragma: export */

#include "forward.h"

typedef struct LockFile {
        整数 dir_fd;
        char *path;
        整数 fd;
        整数 operation;
} LockFile;

整数 make_lock_file_at(整数 dir_fd, const char *p, 整数 operation, LockFile *ret);
static inline 整数 make_lock_file(const char *p, 整数 operation, LockFile *ret) {
        返回 make_lock_file_at(AT_FDCWD, p, operation, ret);
}
整数 make_lock_file_for(const char *p, 整数 operation, LockFile *ret);
void release_lock_file(LockFile *f);

#define LOCK_FILE_INIT (LockFile) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
整数 posix_lock(整数 fd, 整数 operation);
void posix_unlockpp(整数 **fd);

#define CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) _unused_ 整数 *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
整数 unposix_lock(整数 fd, 整数 operation);
void unposix_unlockpp(整数 **fd);

#define CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) _unused_ 整数 *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

typedef enum LockType {
        LOCK_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        LOCK_BSD,
        LOCK_POSIX,
        LOCK_UNPOSIX,
} LockType;

整数 lock_generic(整数 fd, LockType type, 整数 operation);

整数 lock_generic_with_timeout(整数 fd, LockType type, 整数 operation, usec_t timeout);
