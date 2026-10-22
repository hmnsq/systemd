/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#شامل <sys/file.h> /* IWYU pragma: export */

#شامل "forward.h"

تعریفنوع ساختار LockFile {
        صحیح dir_fd;
        نویسه *path;
        صحیح fd;
        صحیح operation;
} LockFile;

صحیح make_lock_file_at(صحیح dir_fd, ثابت نویسه *p, صحیح operation, LockFile *ret);
ایستا درونخطی صحیح make_lock_file(ثابت نویسه *p, صحیح operation, LockFile *ret) {
        بازگشت make_lock_file_at(AT_FDCWD, p, operation, ret);
}
صحیح make_lock_file_for(ثابت نویسه *p, صحیح operation, LockFile *ret);
تهی release_lock_file(LockFile *f);

#define LOCK_FILE_INIT (LockFile) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
صحیح posix_lock(صحیح fd, صحیح operation);
تهی posix_unlockpp(صحیح **fd);

#define CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) _unused_ صحیح *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
صحیح unposix_lock(صحیح fd, صحیح operation);
تهی unposix_unlockpp(صحیح **fd);

#define CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) _unused_ صحیح *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

تعریفنوع شمارش LockType {
        LOCK_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        LOCK_BSD,
        LOCK_POSIX,
        LOCK_UNPOSIX,
} LockType;

صحیح lock_generic(صحیح fd, LockType type, صحیح operation);

صحیح lock_generic_with_timeout(صحیح fd, LockType type, صحیح operation, usec_t timeout);
