/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#include <sys/file.h> /* IWYU pragma: export */

#include "forward.h"

typedef struct LockFile {
        số_nguyên dir_fd;
        char *path;
        số_nguyên fd;
        số_nguyên operation;
} LockFile;

số_nguyên make_lock_file_at(số_nguyên dir_fd, const char *p, số_nguyên operation, LockFile *ret);
static inline số_nguyên make_lock_file(const char *p, số_nguyên operation, LockFile *ret) {
        trả_về make_lock_file_at(AT_FDCWD, p, operation, ret);
}
số_nguyên make_lock_file_for(const char *p, số_nguyên operation, LockFile *ret);
void release_lock_file(LockFile *f);

#define LOCK_FILE_INIT (LockFile) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
số_nguyên posix_lock(số_nguyên fd, số_nguyên operation);
void posix_unlockpp(số_nguyên **fd);

#define CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) _unused_ số_nguyên *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
số_nguyên unposix_lock(số_nguyên fd, số_nguyên operation);
void unposix_unlockpp(số_nguyên **fd);

#define CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) _unused_ số_nguyên *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

typedef enum LockType {
        LOCK_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        LOCK_BSD,
        LOCK_POSIX,
        LOCK_UNPOSIX,
} LockType;

số_nguyên lock_generic(số_nguyên fd, LockType type, số_nguyên operation);

số_nguyên lock_generic_with_timeout(số_nguyên fd, LockType type, số_nguyên operation, usec_t timeout);
