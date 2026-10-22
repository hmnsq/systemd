/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#അന്തര്ഭാവ <sys/file.h> /* IWYU pragma: export */

#അന്തര്ഭാവ "forward.h"

തരനിർവചനം ഘടന LockFile {
        പൂർണ്ണസംഖ്യ dir_fd;
        അക്ഷരം *path;
        പൂർണ്ണസംഖ്യ fd;
        പൂർണ്ണസംഖ്യ operation;
} LockFile;

പൂർണ്ണസംഖ്യ make_lock_file_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *p, പൂർണ്ണസംഖ്യ operation, LockFile *ret);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ make_lock_file(സ്ഥിരം അക്ഷരം *p, പൂർണ്ണസംഖ്യ operation, LockFile *ret) {
        തിരികെ make_lock_file_at(AT_FDCWD, p, operation, ret);
}
പൂർണ്ണസംഖ്യ make_lock_file_for(സ്ഥിരം അക്ഷരം *p, പൂർണ്ണസംഖ്യ operation, LockFile *ret);
ശൂന്യം release_lock_file(LockFile *f);

#define LOCK_FILE_INIT (LockFile) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
പൂർണ്ണസംഖ്യ posix_lock(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ operation);
ശൂന്യം posix_unlockpp(പൂർണ്ണസംഖ്യ **fd);

#define CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) _unused_ പൂർണ്ണസംഖ്യ *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
പൂർണ്ണസംഖ്യ unposix_lock(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ operation);
ശൂന്യം unposix_unlockpp(പൂർണ്ണസംഖ്യ **fd);

#define CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) _unused_ പൂർണ്ണസംഖ്യ *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

തരനിർവചനം എണ്ണൽ LockType {
        LOCK_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        LOCK_BSD,
        LOCK_POSIX,
        LOCK_UNPOSIX,
} LockType;

പൂർണ്ണസംഖ്യ lock_generic(പൂർണ്ണസംഖ്യ fd, LockType type, പൂർണ്ണസംഖ്യ operation);

പൂർണ്ണസംഖ്യ lock_generic_with_timeout(പൂർണ്ണസംഖ്യ fd, LockType type, പൂർണ്ണസംഖ്യ operation, usec_t timeout);
