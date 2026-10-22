/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#شامل <sys/file.h> /* IWYU pragma: export */

#شامل "forward.h"

قسمتعریف ساخت تالا_File {
        صحیح dir_fd;
        حرف *راستہ;
        صحیح fd;
        صحیح operation;
} تالا_File;

صحیح make_تالا_file_at(صحیح dir_fd, ثابت حرف *p, صحیح operation, تالا_File *ret);
ساکن اندرونی صحیح make_تالا_file(ثابت حرف *p, صحیح operation, تالا_File *ret) {
        واپس make_تالا_file_at(AT_FDCWD, p, operation, ret);
}
صحیح make_تالا_file_for(ثابت حرف *p, صحیح operation, تالا_File *ret);
خالی release_تالا_file(تالا_File *f);

#تعریف_کرو تالا_FILE_INIT (تالا_File) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
صحیح posix_تالا(صحیح fd, صحیح operation);
خالی posix_unlockpp(صحیح **fd);

#تعریف_کرو CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) غیر_مستعمل صحیح *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
صحیح unposix_تالا(صحیح fd, صحیح operation);
خالی unposix_unlockpp(صحیح **fd);

#تعریف_کرو CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) غیر_مستعمل صحیح *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

قسمتعریف شمار تالا_Type {
        تالا_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        تالا_BSD,
        تالا_POSIX,
        تالا_UNPOSIX,
} تالا_Type;

صحیح تالا_generic(صحیح fd, تالا_Type type, صحیح operation);

صحیح تالا_generic_with_timeout(صحیح fd, تالا_Type type, صحیح operation, usec_t timeout);
