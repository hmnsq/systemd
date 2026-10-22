/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#अन्तर्भाव <sys/file.h> /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

प्रकारनिर्देश संरचना LockFile {
        पूर्णाङ्क dir_fd;
        वर्ण *path;
        पूर्णाङ्क fd;
        पूर्णाङ्क operation;
} LockFile;

पूर्णाङ्क make_lock_file_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *p, पूर्णाङ्क operation, LockFile *ret);
स्थिर अन्तःस्थ पूर्णाङ्क make_lock_file(ध्रुव वर्ण *p, पूर्णाङ्क operation, LockFile *ret) {
        पटिनिवत्त make_lock_file_at(AT_FDCWD, p, operation, ret);
}
पूर्णाङ्क make_lock_file_for(ध्रुव वर्ण *p, पूर्णाङ्क operation, LockFile *ret);
शून्य release_lock_file(LockFile *f);

#define LOCK_FILE_INIT (LockFile) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
पूर्णाङ्क posix_lock(पूर्णाङ्क fd, पूर्णाङ्क operation);
शून्य posix_unlockpp(पूर्णाङ्क **fd);

#define CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) _unused_ पूर्णाङ्क *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
पूर्णाङ्क unposix_lock(पूर्णाङ्क fd, पूर्णाङ्क operation);
शून्य unposix_unlockpp(पूर्णाङ्क **fd);

#define CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) _unused_ पूर्णाङ्क *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

प्रकारनिर्देश गणना LockType {
        LOCK_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        LOCK_BSD,
        LOCK_POSIX,
        LOCK_UNPOSIX,
} LockType;

पूर्णाङ्क lock_generic(पूर्णाङ्क fd, LockType type, पूर्णाङ्क operation);

पूर्णाङ्क lock_generic_with_timeout(पूर्णाङ्क fd, LockType type, पूर्णाङ्क operation, usec_t timeout);
