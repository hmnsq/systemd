/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#ضمن <sys/file.h> /* IWYU pragma: export */

#ضمن "forward.h"

تعريفنوع بنية جندرا_File {
        صحيح dir_fd;
        حرف *رستہ;
        صحيح fd;
        صحيح operation;
} جندرا_File;

صحيح make_جندرا_file_at(صحيح dir_fd, ثابت حرف *p, صحيح operation, جندرا_File *ret);
ساكن مضمن صحيح make_جندرا_file(ثابت حرف *p, صحيح operation, جندرا_File *ret) {
        أرجع make_جندرا_file_at(AT_FDCWD, p, operation, ret);
}
صحيح make_جندرا_file_for(ثابت حرف *p, صحيح operation, جندرا_File *ret);
فراغ release_جندرا_file(جندرا_File *f);

#تعریف_کرو جندرا_FILE_INIT (جندرا_File) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
صحيح posix_جندرا(صحيح fd, صحيح operation);
فراغ posix_unlockpp(صحيح **fd);

#تعریف_کرو CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) بن_ورتیا صحيح *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
صحيح unposix_جندرا(صحيح fd, صحيح operation);
فراغ unposix_unlockpp(صحيح **fd);

#تعریف_کرو CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) بن_ورتیا صحيح *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

تعريفنوع تعداد جندرا_Type {
        جندرا_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        جندرا_BSD,
        جندرا_POSIX,
        جندرا_UNPOSIX,
} جندرا_Type;

صحيح جندرا_generic(صحيح fd, جندرا_Type type, صحيح operation);

صحيح جندرا_generic_with_timeout(صحيح fd, جندرا_Type type, صحيح operation, usec_t timeout);
