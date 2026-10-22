/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#include <sys/file.h> /* IWYU pragma: export */

#include "forward.h"

typedef sa_nracanaa LockFile {
        පූර්ණ_සංඛ්‍යා dir_fd;
        var_na *path;
        පූර්ණ_සංඛ්‍යා fd;
        පූර්ණ_සංඛ්‍යා operation;
} LockFile;

පූර්ණ_සංඛ්‍යා make_lock_file_at(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *p, පූර්ණ_සංඛ්‍යා operation, LockFile *ret);
static anta_hstha පූර්ණ_සංඛ්‍යා make_lock_file(const var_na *p, පූර්ණ_සංඛ්‍යා operation, LockFile *ret) {
        ආපසු make_lock_file_at(AT_FDCWD, p, operation, ret);
}
පූර්ණ_සංඛ්‍යා make_lock_file_for(const var_na *p, පූර්ණ_සංඛ්‍යා operation, LockFile *ret);
void release_lock_file(LockFile *f);

#define LOCK_FILE_INIT (LockFile) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
පූර්ණ_සංඛ්‍යා posix_lock(පූර්ණ_සංඛ්‍යා fd, පූර්ණ_සංඛ්‍යා operation);
void posix_unlockpp(පූර්ණ_සංඛ්‍යා **fd);

#define CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) _unused_ පූර්ණ_සංඛ්‍යා *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
පූර්ණ_සංඛ්‍යා unposix_lock(පූර්ණ_සංඛ්‍යා fd, පූර්ණ_සංඛ්‍යා operation);
void unposix_unlockpp(පූර්ණ_සංඛ්‍යා **fd);

#define CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) _unused_ පූර්ණ_සංඛ්‍යා *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

typedef ga_nanaa LockType {
        LOCK_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        LOCK_BSD,
        LOCK_POSIX,
        LOCK_UNPOSIX,
} LockType;

පූර්ණ_සංඛ්‍යා lock_generic(පූර්ණ_සංඛ්‍යා fd, LockType type, පූර්ණ_සංඛ්‍යා operation);

පූර්ණ_සංඛ්‍යා lock_generic_with_timeout(පූර්ණ_සංඛ්‍යා fd, LockType type, පූර්ණ_සංඛ්‍යා operation, usec_t timeout);
