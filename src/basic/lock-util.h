/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#ضمن <sys/file.h> /* IWYU pragma: export */

#ضمن "forward.h"

تعريفنوع بنية قلف_File {
        صحيح dir_fd;
        حرف *لار;
        صحيح fd;
        صحيح operation;
} قلف_File;

صحيح make_قلف_file_at(صحيح dir_fd, ثابت حرف *p, صحيح operation, قلف_File *ret);
ساكن مضمن صحيح make_قلف_file(ثابت حرف *p, صحيح operation, قلف_File *ret) {
        أرجع make_قلف_file_at(AT_FDCWD, p, operation, ret);
}
صحيح make_قلف_file_for(ثابت حرف *p, صحيح operation, قلف_File *ret);
فراغ release_قلف_file(قلف_File *f);

#تعریف_کړه قلف_FILE_INIT (قلف_File) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
صحيح posix_قلف(صحيح fd, صحيح operation);
فراغ posix_unlockpp(صحيح **fd);

#تعریف_کړه CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) ناکارول_شوی صحيح *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
صحيح unposix_قلف(صحيح fd, صحيح operation);
فراغ unposix_unlockpp(صحيح **fd);

#تعریف_کړه CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) ناکارول_شوی صحيح *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

تعريفنوع تعداد قلف_Type {
        قلف_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        قلف_BSD,
        قلف_POSIX,
        قلف_UNPOSIX,
} قلف_Type;

صحيح قلف_generic(صحيح fd, قلف_Type type, صحيح operation);

صحيح قلف_generic_with_timeout(صحيح fd, قلف_Type type, صحيح operation, usec_t timeout);
