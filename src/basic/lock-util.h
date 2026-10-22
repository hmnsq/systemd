/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#ضمن <sys/file.h> /* IWYU pragma: export */

#ضمن "forward.h"

تعريفنوع بنية تالو_File {
        صحيح dir_fd;
        حرف *رستو;
        صحيح fd;
        صحيح operation;
} تالو_File;

صحيح make_تالو_file_at(صحيح dir_fd, ثابت حرف *p, صحيح operation, تالو_File *ret);
ساكن مضمن صحيح make_تالو_file(ثابت حرف *p, صحيح operation, تالو_File *ret) {
        أرجع make_تالو_file_at(AT_FDCWD, p, operation, ret);
}
صحيح make_تالو_file_for(ثابت حرف *p, صحيح operation, تالو_File *ret);
فراغ release_تالو_file(تالو_File *f);

#وصف_ڪريو تالو_FILE_INIT (تالو_File) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
صحيح posix_تالو(صحيح fd, صحيح operation);
فراغ posix_unlockpp(صحيح **fd);

#وصف_ڪريو CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) اڻ_استعمال_ٿيل صحيح *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
صحيح unposix_تالو(صحيح fd, صحيح operation);
فراغ unposix_unlockpp(صحيح **fd);

#وصف_ڪريو CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) اڻ_استعمال_ٿيل صحيح *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

تعريفنوع تعداد تالو_Type {
        تالو_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        تالو_BSD,
        تالو_POSIX,
        تالو_UNPOSIX,
} تالو_Type;

صحيح تالو_generic(صحيح fd, تالو_Type type, صحيح operation);

صحيح تالو_generic_with_timeout(صحيح fd, تالو_Type type, صحيح operation, usec_t timeout);
