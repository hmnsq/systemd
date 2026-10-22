/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#અન્તર્ભાવ <sys/file.h> /* IWYU pragma: export */

#અન્તર્ભાવ "forward.h"

પ્રકારવ્યાખ્યા સંરચના LockFile {
        પૂર્ણાંક dir_fd;
        અક્ષર *path;
        પૂર્ણાંક fd;
        પૂર્ણાંક operation;
} LockFile;

પૂર્ણાંક make_lock_file_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *p, પૂર્ણાંક operation, LockFile *ret);
સ્થિર અંતઃરેખ પૂર્ણાંક make_lock_file(અચલ અક્ષર *p, પૂર્ણાંક operation, LockFile *ret) {
        પાછા make_lock_file_at(AT_FDCWD, p, operation, ret);
}
પૂર્ણાંક make_lock_file_for(અચલ અક્ષર *p, પૂર્ણાંક operation, LockFile *ret);
શૂન્ય release_lock_file(LockFile *f);

#define LOCK_FILE_INIT (LockFile) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
પૂર્ણાંક posix_lock(પૂર્ણાંક fd, પૂર્ણાંક operation);
શૂન્ય posix_unlockpp(પૂર્ણાંક **fd);

#define CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) _unused_ પૂર્ણાંક *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
પૂર્ણાંક unposix_lock(પૂર્ણાંક fd, પૂર્ણાંક operation);
શૂન્ય unposix_unlockpp(પૂર્ણાંક **fd);

#define CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) _unused_ પૂર્ણાંક *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

પ્રકારવ્યાખ્યા ગણના LockType {
        LOCK_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        LOCK_BSD,
        LOCK_POSIX,
        LOCK_UNPOSIX,
} LockType;

પૂર્ણાંક lock_generic(પૂર્ણાંક fd, LockType type, પૂર્ણાંક operation);

પૂર્ણાંક lock_generic_with_timeout(પૂર્ણાંક fd, LockType type, પૂર્ણાંક operation, usec_t timeout);
