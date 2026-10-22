/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#ضمن <sys/file.h> /* IWYU pragma: export */

#ضمن "forward.h"

تعريفنوع بنية LockFile {
        صحيح dir_fd;
        حرف *path;
        صحيح fd;
        صحيح operation;
} LockFile;

صحيح make_lock_file_at(صحيح dir_fd, ثابت حرف *p, صحيح operation, LockFile *ret);
ساكن مضمن صحيح make_lock_file(ثابت حرف *p, صحيح operation, LockFile *ret) {
        ܦܢܐ make_lock_file_at(AT_FDCWD, p, operation, ret);
}
صحيح make_lock_file_for(ثابت حرف *p, صحيح operation, LockFile *ret);
فراغ release_lock_file(LockFile *f);

#define LOCK_FILE_INIT (LockFile) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
صحيح posix_lock(صحيح fd, صحيح operation);
فراغ posix_unlockpp(صحيح **fd);

#define CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) _unused_ صحيح *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
صحيح unposix_lock(صحيح fd, صحيح operation);
فراغ unposix_unlockpp(صحيح **fd);

#define CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) _unused_ صحيح *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

تعريفنوع تعداد LockType {
        LOCK_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        LOCK_BSD,
        LOCK_POSIX,
        LOCK_UNPOSIX,
} LockType;

صحيح lock_generic(صحيح fd, LockType type, صحيح operation);

صحيح lock_generic_with_timeout(صحيح fd, LockType type, صحيح operation, usec_t timeout);
