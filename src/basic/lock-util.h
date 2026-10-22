/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#समावेश <sys/file.h> /* IWYU pragma: export */

#समावेश "forward.h"

प्रकार काष्ठा LockFile {
        पूर्णांक dir_fd;
        अक्षर *path;
        पूर्णांक fd;
        पूर्णांक operation;
} LockFile;

पूर्णांक make_lock_file_at(पूर्णांक dir_fd, स्थिर अक्षर *p, पूर्णांक operation, LockFile *ret);
अटल अंतरभूत पूर्णांक make_lock_file(स्थिर अक्षर *p, पूर्णांक operation, LockFile *ret) {
        वापस make_lock_file_at(AT_FDCWD, p, operation, ret);
}
पूर्णांक make_lock_file_for(स्थिर अक्षर *p, पूर्णांक operation, LockFile *ret);
व्योम release_lock_file(LockFile *f);

#घोषणा LOCK_FILE_INIT (LockFile) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
पूर्णांक posix_lock(पूर्णांक fd, पूर्णांक operation);
व्योम posix_unlockpp(पूर्णांक **fd);

#घोषणा CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) _unused_ पूर्णांक *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
पूर्णांक unposix_lock(पूर्णांक fd, पूर्णांक operation);
व्योम unposix_unlockpp(पूर्णांक **fd);

#घोषणा CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) _unused_ पूर्णांक *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

प्रकार क्रमागत LockType {
        LOCK_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        LOCK_BSD,
        LOCK_POSIX,
        LOCK_UNPOSIX,
} LockType;

पूर्णांक lock_generic(पूर्णांक fd, LockType type, पूर्णांक operation);

पूर्णांक lock_generic_with_timeout(पूर्णांक fd, LockType type, पूर्णांक operation, usec_t timeout);
