/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#include <sys/file.h> /* IWYU pragma: export */

#include "forward.h"

typedef sa_nracanaa LockFile {
        puur_naa_nka dir_fd;
        var_na *path;
        puur_naa_nka fd;
        puur_naa_nka operation;
} LockFile;

puur_naa_nka make_lock_file_at(puur_naa_nka dir_fd, const var_na *p, puur_naa_nka operation, LockFile *ret);
static anta_hstha puur_naa_nka make_lock_file(const var_na *p, puur_naa_nka operation, LockFile *ret) {
        ᮝᮜᮤ make_lock_file_at(AT_FDCWD, p, operation, ret);
}
puur_naa_nka make_lock_file_for(const var_na *p, puur_naa_nka operation, LockFile *ret);
void release_lock_file(LockFile *f);

#define LOCK_FILE_INIT (LockFile) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
puur_naa_nka posix_lock(puur_naa_nka fd, puur_naa_nka operation);
void posix_unlockpp(puur_naa_nka **fd);

#define CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) _unused_ puur_naa_nka *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
puur_naa_nka unposix_lock(puur_naa_nka fd, puur_naa_nka operation);
void unposix_unlockpp(puur_naa_nka **fd);

#define CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) _unused_ puur_naa_nka *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

typedef ga_nanaa LockType {
        LOCK_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        LOCK_BSD,
        LOCK_POSIX,
        LOCK_UNPOSIX,
} LockType;

puur_naa_nka lock_generic(puur_naa_nka fd, LockType type, puur_naa_nka operation);

puur_naa_nka lock_generic_with_timeout(puur_naa_nka fd, LockType type, puur_naa_nka operation, usec_t timeout);
