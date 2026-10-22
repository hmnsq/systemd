/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#ضمن <sys/file.h> /* IWYU pragma: export */

#ضمن "forward.h"

تعريفنوع بنية قفل_File {
        صحيح dir_fd;
        حرف *مسیر;
        صحيح fd;
        صحيح operation;
} قفل_File;

صحيح make_قفل_file_at(صحيح dir_fd, ثابت حرف *p, صحيح operation, قفل_File *ret);
ساكن مضمن صحيح make_قفل_file(ثابت حرف *p, صحيح operation, قفل_File *ret) {
        أرجع make_قفل_file_at(AT_FDCWD, p, operation, ret);
}
صحيح make_قفل_file_for(ثابت حرف *p, صحيح operation, قفل_File *ret);
فراغ release_قفل_file(قفل_File *f);

#تعریف_کن قفل_FILE_INIT (قفل_File) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
صحيح posix_قفل(صحيح fd, صحيح operation);
فراغ posix_unlockpp(صحيح **fd);

#تعریف_کن CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) استفادهنشده صحيح *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
صحيح unposix_قفل(صحيح fd, صحيح operation);
فراغ unposix_unlockpp(صحيح **fd);

#تعریف_کن CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) استفادهنشده صحيح *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

تعريفنوع تعداد قفل_Type {
        قفل_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        قفل_BSD,
        قفل_POSIX,
        قفل_UNPOSIX,
} قفل_Type;

صحيح قفل_generic(صحيح fd, قفل_Type type, صحيح operation);

صحيح قفل_generic_with_timeout(صحيح fd, قفل_Type type, صحيح operation, usec_t timeout);
