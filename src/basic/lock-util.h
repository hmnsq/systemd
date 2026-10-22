/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#अन्तर्भाव <sys/file.h> /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

प्रकारनिर्देश संरचना तालकम्_File {
        पूर्णाङ्क dir_fd;
        वर्ण *मार्गः;
        पूर्णाङ्क fd;
        पूर्णाङ्क operation;
} तालकम्_File;

पूर्णाङ्क make_तालकम्_file_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *p, पूर्णाङ्क operation, तालकम्_File *ret);
स्थिर अन्तःस्थ पूर्णाङ्क make_तालकम्_file(ध्रुव वर्ण *p, पूर्णाङ्क operation, तालकम्_File *ret) {
        प्रत्यागम make_तालकम्_file_at(AT_FDCWD, p, operation, ret);
}
पूर्णाङ्क make_तालकम्_file_for(ध्रुव वर्ण *p, पूर्णाङ्क operation, तालकम्_File *ret);
शून्य release_तालकम्_file(तालकम्_File *f);

#परिभाषय तालकम्_FILE_INIT (तालकम्_File) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
पूर्णाङ्क posix_तालकम्(पूर्णाङ्क fd, पूर्णाङ्क operation);
शून्य posix_unlockpp(पूर्णाङ्क **fd);

#परिभाषय CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) अप्रयुक्तम् पूर्णाङ्क *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
पूर्णाङ्क unposix_तालकम्(पूर्णाङ्क fd, पूर्णाङ्क operation);
शून्य unposix_unlockpp(पूर्णाङ्क **fd);

#परिभाषय CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) अप्रयुक्तम् पूर्णाङ्क *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

प्रकारनिर्देश गणना तालकम्_Type {
        तालकम्_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        तालकम्_BSD,
        तालकम्_POSIX,
        तालकम्_UNPOSIX,
} तालकम्_Type;

पूर्णाङ्क तालकम्_generic(पूर्णाङ्क fd, तालकम्_Type type, पूर्णाङ्क operation);

पूर्णाङ्क तालकम्_generic_with_timeout(पूर्णाङ्क fd, तालकम्_Type type, पूर्णाङ्क operation, usec_t timeout);
