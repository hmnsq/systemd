/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <dirent.h>             /* IWYU pragma: export */

#অন্তর্ভাব "forward.h"
#অন্তর্ভাব "path-util.h"          /* IWYU pragma: keep */

bool dirent_is_file(ধ্রুবক কাঠামো dirent *de) _pure_;
bool dirent_is_file_with_suffix(ধ্রুবক কাঠামো dirent *de, ধ্রুবক অক্ষর *suffix) _pure_;
পূর্ণসংখ্যা dirent_ensure_type(পূর্ণসংখ্যা dir_fd, কাঠামো dirent *de);

কাঠামো dirent* readdir_ensure_type(DIR *d);
কাঠামো dirent* readdir_no_dot(DIR *dirp);

#define FOREACH_DIRENT_ALL(de, d, on_error)                             \
        জন্য (কাঠামো dirent *(de) = readdir_ensure_type(d);; (de) = readdir_ensure_type(d)) \
                যদি (!de) {                                              \
                        যদি (errno > 0) {                                \
                                on_error;                               \
                        }                                               \
                        বিরতি;                                          \
                } অন্যথা

#define FOREACH_DIRENT(de, d, on_error)                                 \
        FOREACH_DIRENT_ALL(de, d, on_error)                             \
             যদি (hidden_or_backup_file((de)->d_name))                   \
                     চালু;                                          \
             অন্যথা

/* Maximum space one dirent structure might require at most */
#define DIRENT_আকার_MAX CONST_MAX(আকার(কাঠামো dirent), offsetof(কাঠামো dirent, d_name) + NAME_MAX + 1)

/* Only if 64-bit off_t is enabled struct dirent + struct dirent64 are actually the same. We require this, and
 * we want them to be interchangeable to make getdents64() work, hence verify that. */
assert_cc(_FILE_OFFSET_BITS == 64);
assert_cc(আকার(কাঠামো dirent) == আকার(কাঠামো dirent64));
assert_cc(offsetof(কাঠামো dirent, d_ino) == offsetof(কাঠামো dirent64, d_ino));
assert_cc(sizeof_field(কাঠামো dirent, d_ino) == sizeof_field(কাঠামো dirent64, d_ino));
assert_cc(offsetof(কাঠামো dirent, d_off) == offsetof(কাঠামো dirent64, d_off));
assert_cc(sizeof_field(কাঠামো dirent, d_off) == sizeof_field(কাঠামো dirent64, d_off));
assert_cc(offsetof(কাঠামো dirent, d_reclen) == offsetof(কাঠামো dirent64, d_reclen));
assert_cc(sizeof_field(কাঠামো dirent, d_reclen) == sizeof_field(কাঠামো dirent64, d_reclen));
assert_cc(offsetof(কাঠামো dirent, d_type) == offsetof(কাঠামো dirent64, d_type));
assert_cc(sizeof_field(কাঠামো dirent, d_type) == sizeof_field(কাঠামো dirent64, d_type));
assert_cc(offsetof(কাঠামো dirent, d_name) == offsetof(কাঠামো dirent64, d_name));
assert_cc(sizeof_field(কাঠামো dirent, d_name) == sizeof_field(কাঠামো dirent64, d_name));

#define FOREACH_DIRENT_IN_বাফার(de, buf, sz)                           \
        জন্য (শূন্য *_end = (uint8_t*) ({ (de) = (buf); }) + (sz);        \
             (uint8_t*) (de) < (uint8_t*) _end;                         \
             (de) = (কাঠামো dirent*) ((uint8_t*) (de) + (de)->d_reclen))

#define DEFINE_DIRENT_বাফার(name, sz)                                  \
        সংঘ {                                                         \
                কাঠামো dirent de;                                       \
                uint8_t তথ্য[(sz) * DIRENT_আকার_MAX];                   \
        } name
