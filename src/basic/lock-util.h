/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#ಅನ್ತರ್ಭಾವ <sys/file.h> /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ LockFile {
        ಪೂರ್ಣಾಙ್ಕ dir_fd;
        ವರ್ಣ *path;
        ಪೂರ್ಣಾಙ್ಕ fd;
        ಪೂರ್ಣಾಙ್ಕ operation;
} LockFile;

ಪೂರ್ಣಾಙ್ಕ make_lock_file_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *p, ಪೂರ್ಣಾಙ್ಕ operation, LockFile *ret);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ make_lock_file(ಧ್ರುವ ವರ್ಣ *p, ಪೂರ್ಣಾಙ್ಕ operation, LockFile *ret) {
        ಪ್ರತ್ಯಾಗಮ make_lock_file_at(AT_FDCWD, p, operation, ret);
}
ಪೂರ್ಣಾಙ್ಕ make_lock_file_for(ಧ್ರುವ ವರ್ಣ *p, ಪೂರ್ಣಾಙ್ಕ operation, LockFile *ret);
ಶೂನ್ಯ release_lock_file(LockFile *f);

#define LOCK_FILE_INIT (LockFile) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
ಪೂರ್ಣಾಙ್ಕ posix_lock(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ operation);
ಶೂನ್ಯ posix_unlockpp(ಪೂರ್ಣಾಙ್ಕ **fd);

#define CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) _unused_ ಪೂರ್ಣಾಙ್ಕ *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
ಪೂರ್ಣಾಙ್ಕ unposix_lock(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ operation);
ಶೂನ್ಯ unposix_unlockpp(ಪೂರ್ಣಾಙ್ಕ **fd);

#define CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) _unused_ ಪೂರ್ಣಾಙ್ಕ *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ LockType {
        LOCK_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        LOCK_BSD,
        LOCK_POSIX,
        LOCK_UNPOSIX,
} LockType;

ಪೂರ್ಣಾಙ್ಕ lock_generic(ಪೂರ್ಣಾಙ್ಕ fd, LockType type, ಪೂರ್ಣಾಙ್ಕ operation);

ಪೂರ್ಣಾಙ್ಕ lock_generic_with_timeout(ಪೂರ್ಣಾಙ್ಕ fd, LockType type, ಪೂರ್ಣಾಙ್ಕ operation, usec_t timeout);
