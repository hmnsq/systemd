/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#அந்தர்பாவ <sys/file.h> /* IWYU pragma: export */

#அந்தர்பாவ "forward.h"

வகைவரை கட்டமைப்பு LockFile {
        முழு dir_fd;
        எழுத்து *path;
        முழு fd;
        முழு operation;
} LockFile;

முழு make_lock_file_at(முழு dir_fd, மாறிலி எழுத்து *p, முழு operation, LockFile *ret);
நிலையான உள்வரி முழு make_lock_file(மாறிலி எழுத்து *p, முழு operation, LockFile *ret) {
        திரும்பு make_lock_file_at(AT_FDCWD, p, operation, ret);
}
முழு make_lock_file_for(மாறிலி எழுத்து *p, முழு operation, LockFile *ret);
வெறுமை release_lock_file(LockFile *f);

#define LOCK_FILE_INIT (LockFile) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
முழு posix_lock(முழு fd, முழு operation);
வெறுமை posix_unlockpp(முழு **fd);

#define CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) _unused_ முழு *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
முழு unposix_lock(முழு fd, முழு operation);
வெறுமை unposix_unlockpp(முழு **fd);

#define CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) _unused_ முழு *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

வகைவரை எண்ணிக்கை LockType {
        LOCK_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        LOCK_BSD,
        LOCK_POSIX,
        LOCK_UNPOSIX,
} LockType;

முழு lock_generic(முழு fd, LockType type, முழு operation);

முழு lock_generic_with_timeout(முழு fd, LockType type, முழு operation, usec_t timeout);
