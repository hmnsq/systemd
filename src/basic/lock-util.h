/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#ಅನ್ತರ್ಭಾವ <sys/file.h> /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಬೀಗ_File {
        ಪೂರ್ಣಾಂಕ dir_fd;
        ಅಕ್ಷರ *ಮಾರ್ಗ;
        ಪೂರ್ಣಾಂಕ fd;
        ಪೂರ್ಣಾಂಕ operation;
} ಬೀಗ_File;

ಪೂರ್ಣಾಂಕ make_ಬೀಗ_file_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಪೂರ್ಣಾಂಕ operation, ಬೀಗ_File *ret);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ make_ಬೀಗ_file(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಪೂರ್ಣಾಂಕ operation, ಬೀಗ_File *ret) {
        ಹಿಂತಿರುಗಿ make_ಬೀಗ_file_at(AT_FDCWD, p, operation, ret);
}
ಪೂರ್ಣಾಂಕ make_ಬೀಗ_file_for(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಪೂರ್ಣಾಂಕ operation, ಬೀಗ_File *ret);
ಶೂನ್ಯ release_ಬೀಗ_file(ಬೀಗ_File *f);

#ವ್ಯಾಖ್ಯಾನಿಸು ಬೀಗ_FILE_INIT (ಬೀಗ_File) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
ಪೂರ್ಣಾಂಕ posix_ಬೀಗ(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ operation);
ಶೂನ್ಯ posix_unlockpp(ಪೂರ್ಣಾಂಕ **fd);

#ವ್ಯಾಖ್ಯಾನಿಸು CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) ಬಳಸದ ಪೂರ್ಣಾಂಕ *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
ಪೂರ್ಣಾಂಕ unposix_ಬೀಗ(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ operation);
ಶೂನ್ಯ unposix_unlockpp(ಪೂರ್ಣಾಂಕ **fd);

#ವ್ಯಾಖ್ಯಾನಿಸು CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) ಬಳಸದ ಪೂರ್ಣಾಂಕ *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ಬೀಗ_Type {
        ಬೀಗ_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        ಬೀಗ_BSD,
        ಬೀಗ_POSIX,
        ಬೀಗ_UNPOSIX,
} ಬೀಗ_Type;

ಪೂರ್ಣಾಂಕ ಬೀಗ_generic(ಪೂರ್ಣಾಂಕ fd, ಬೀಗ_Type type, ಪೂರ್ಣಾಂಕ operation);

ಪೂರ್ಣಾಂಕ ಬೀಗ_generic_with_timeout(ಪೂರ್ಣಾಂಕ fd, ಬೀಗ_Type type, ಪೂರ್ಣಾಂಕ operation, usec_t timeout);
