/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#অন্তর্ভাব <sys/file.h> /* IWYU pragma: export */

#অন্তর্ভাব "forward.h"

প্রকারলেখ কাঠামো LockFile {
        পূর্ণসংখ্যা dir_fd;
        অক্ষর *path;
        পূর্ণসংখ্যা fd;
        পূর্ণসংখ্যা operation;
} LockFile;

পূর্ণসংখ্যা make_lock_file_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *p, পূর্ণসংখ্যা operation, LockFile *ret);
স্থির অন্তঃরেখ পূর্ণসংখ্যা make_lock_file(ধ্রুবক অক্ষর *p, পূর্ণসংখ্যা operation, LockFile *ret) {
        ফেরত make_lock_file_at(AT_FDCWD, p, operation, ret);
}
পূর্ণসংখ্যা make_lock_file_for(ধ্রুবক অক্ষর *p, পূর্ণসংখ্যা operation, LockFile *ret);
শূন্য release_lock_file(LockFile *f);

#define LOCK_FILE_INIT (LockFile) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
পূর্ণসংখ্যা posix_lock(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা operation);
শূন্য posix_unlockpp(পূর্ণসংখ্যা **fd);

#define CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) _unused_ পূর্ণসংখ্যা *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
পূর্ণসংখ্যা unposix_lock(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা operation);
শূন্য unposix_unlockpp(পূর্ণসংখ্যা **fd);

#define CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) _unused_ পূর্ণসংখ্যা *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

প্রকারলেখ গণনা LockType {
        LOCK_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        LOCK_BSD,
        LOCK_POSIX,
        LOCK_UNPOSIX,
} LockType;

পূর্ণসংখ্যা lock_generic(পূর্ণসংখ্যা fd, LockType type, পূর্ণসংখ্যা operation);

পূর্ণসংখ্যা lock_generic_with_timeout(পূর্ণসংখ্যা fd, LockType type, পূর্ণসংখ্যা operation, usec_t timeout);
