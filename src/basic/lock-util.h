/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#include <sys/file.h> /* IWYU pragma: export */

#include "forward.h"

typedef sa_nracanaa LockFile {
        จำนวนเต็ม dir_fd;
        var_na *path;
        จำนวนเต็ม fd;
        จำนวนเต็ม operation;
} LockFile;

จำนวนเต็ม make_lock_file_at(จำนวนเต็ม dir_fd, const var_na *p, จำนวนเต็ม operation, LockFile *ret);
static anta_hstha จำนวนเต็ม make_lock_file(const var_na *p, จำนวนเต็ม operation, LockFile *ret) {
        คืน make_lock_file_at(AT_FDCWD, p, operation, ret);
}
จำนวนเต็ม make_lock_file_for(const var_na *p, จำนวนเต็ม operation, LockFile *ret);
void release_lock_file(LockFile *f);

#define LOCK_FILE_INIT (LockFile) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
จำนวนเต็ม posix_lock(จำนวนเต็ม fd, จำนวนเต็ม operation);
void posix_unlockpp(จำนวนเต็ม **fd);

#define CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) _unused_ จำนวนเต็ม *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
จำนวนเต็ม unposix_lock(จำนวนเต็ม fd, จำนวนเต็ม operation);
void unposix_unlockpp(จำนวนเต็ม **fd);

#define CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) _unused_ จำนวนเต็ม *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

typedef ga_nanaa LockType {
        LOCK_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        LOCK_BSD,
        LOCK_POSIX,
        LOCK_UNPOSIX,
} LockType;

จำนวนเต็ม lock_generic(จำนวนเต็ม fd, LockType type, จำนวนเต็ม operation);

จำนวนเต็ม lock_generic_with_timeout(จำนวนเต็ม fd, LockType type, จำนวนเต็ม operation, usec_t timeout);
