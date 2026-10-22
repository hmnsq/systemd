/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#অন্তর্ভাৱ <sys/file.h> /* IWYU pragma: export */

#অন্তর্ভাৱ "forward.h"

প্রকারনির্দেশ সংরচনা LockFile {
        পূর্ণাঙ্ক dir_fd;
        ৱর্ণ *path;
        পূর্ণাঙ্ক fd;
        পূর্ণাঙ্ক operation;
} LockFile;

পূর্ণাঙ্ক make_lock_file_at(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *p, পূর্ণাঙ্ক operation, LockFile *ret);
স্থির অন্তঃস্থ পূর্ণাঙ্ক make_lock_file(ধ্রুৱ ৱর্ণ *p, পূর্ণাঙ্ক operation, LockFile *ret) {
        ঘূৰাই make_lock_file_at(AT_FDCWD, p, operation, ret);
}
পূর্ণাঙ্ক make_lock_file_for(ধ্রুৱ ৱর্ণ *p, পূর্ণাঙ্ক operation, LockFile *ret);
শূন্য release_lock_file(LockFile *f);

#define LOCK_FILE_INIT (LockFile) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
পূর্ণাঙ্ক posix_lock(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক operation);
শূন্য posix_unlockpp(পূর্ণাঙ্ক **fd);

#define CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) _unused_ পূর্ণাঙ্ক *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
পূর্ণাঙ্ক unposix_lock(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক operation);
শূন্য unposix_unlockpp(পূর্ণাঙ্ক **fd);

#define CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) _unused_ পূর্ণাঙ্ক *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

প্রকারনির্দেশ গণনা LockType {
        LOCK_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        LOCK_BSD,
        LOCK_POSIX,
        LOCK_UNPOSIX,
} LockType;

পূর্ণাঙ্ক lock_generic(পূর্ণাঙ্ক fd, LockType type, পূর্ণাঙ্ক operation);

পূর্ণাঙ্ক lock_generic_with_timeout(পূর্ণাঙ্ক fd, LockType type, পূর্ণাঙ্ক operation, usec_t timeout);
