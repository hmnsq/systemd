/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#ଅନ୍ତର୍ଭାଵ <sys/file.h> /* IWYU pragma: export */

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା LockFile {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd;
        ଅକ୍ଷର *path;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା operation;
} LockFile;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା make_lock_file_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା operation, LockFile *ret);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା make_lock_file(ଅଚଳ ଅକ୍ଷର *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା operation, LockFile *ret) {
        ଫେରନ୍ତୁ make_lock_file_at(AT_FDCWD, p, operation, ret);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା make_lock_file_for(ଅଚଳ ଅକ୍ଷର *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା operation, LockFile *ret);
ଶୂନ୍ୟ release_lock_file(LockFile *f);

#define LOCK_FILE_INIT (LockFile) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା posix_lock(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା operation);
ଶୂନ୍ୟ posix_unlockpp(ପୂର୍ଣ୍ଣସଂଖ୍ୟା **fd);

#define CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) _unused_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା unposix_lock(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା operation);
ଶୂନ୍ୟ unposix_unlockpp(ପୂର୍ଣ୍ଣସଂଖ୍ୟା **fd);

#define CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) _unused_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା LockType {
        LOCK_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        LOCK_BSD,
        LOCK_POSIX,
        LOCK_UNPOSIX,
} LockType;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା lock_generic(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, LockType type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା operation);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା lock_generic_with_timeout(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, LockType type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା operation, usec_t timeout);
