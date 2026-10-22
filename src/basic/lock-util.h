/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/file.h> /* IWYU pragma: export */

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਤਾਲਾ_File {
        ਪੂਰਨ dir_fd;
        ਅੱਖਰ *ਰਸਤਾ;
        ਪੂਰਨ fd;
        ਪੂਰਨ operation;
} ਤਾਲਾ_File;

ਪੂਰਨ make_ਤਾਲਾ_file_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *p, ਪੂਰਨ operation, ਤਾਲਾ_File *ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ make_ਤਾਲਾ_file(ਅਚਲ ਅੱਖਰ *p, ਪੂਰਨ operation, ਤਾਲਾ_File *ret) {
        ਵਾਪਸ make_ਤਾਲਾ_file_at(AT_FDCWD, p, operation, ret);
}
ਪੂਰਨ make_ਤਾਲਾ_file_for(ਅਚਲ ਅੱਖਰ *p, ਪੂਰਨ operation, ਤਾਲਾ_File *ret);
ਖਾਲੀ release_ਤਾਲਾ_file(ਤਾਲਾ_File *f);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਤਾਲਾ_FILE_INIT (ਤਾਲਾ_File) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
ਪੂਰਨ posix_ਤਾਲਾ(ਪੂਰਨ fd, ਪੂਰਨ operation);
ਖਾਲੀ posix_unlockpp(ਪੂਰਨ **fd);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) ਅਣਵਰਤਿਆ ਪੂਰਨ *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
ਪੂਰਨ unposix_ਤਾਲਾ(ਪੂਰਨ fd, ਪੂਰਨ operation);
ਖਾਲੀ unposix_unlockpp(ਪੂਰਨ **fd);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) ਅਣਵਰਤਿਆ ਪੂਰਨ *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ਤਾਲਾ_Type {
        ਤਾਲਾ_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        ਤਾਲਾ_BSD,
        ਤਾਲਾ_POSIX,
        ਤਾਲਾ_UNPOSIX,
} ਤਾਲਾ_Type;

ਪੂਰਨ ਤਾਲਾ_generic(ਪੂਰਨ fd, ਤਾਲਾ_Type type, ਪੂਰਨ operation);

ਪੂਰਨ ਤਾਲਾ_generic_with_timeout(ਪੂਰਨ fd, ਤਾਲਾ_Type type, ਪੂਰਨ operation, usec_t timeout);
