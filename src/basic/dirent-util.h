/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <dirent.h>             /* IWYU pragma: export */

#অন্তর্ভাব "forward.h"
#অন্তর্ভাব "path-util.h"          /* IWYU pragma: keep */

bool dirent_is_file(ধ্রুব সংরচনা dirent *de) _pure_;
bool dirent_is_file_with_suffix(ধ্রুব সংরচনা dirent *de, ধ্রুব বর্ণ *suffix) _pure_;
পূর্ণাঙ্ক dirent_ensure_type(পূর্ণাঙ্ক dir_fd, সংরচনা dirent *de);

সংরচনা dirent* readdir_ensure_type(DIR *d);
সংরচনা dirent* readdir_no_dot(DIR *dirp);

#define FOREACH_DIRENT_ALL(de, d, on_error)                             \
        প্রতি (সংরচনা dirent *(de) = readdir_ensure_type(d);; (de) = readdir_ensure_type(d)) \
                যদি (!de) {                                              \
                        যদি (errno > 0) {                                \
                                on_error;                               \
                        }                                               \
                        ভঙ্গ;                                          \
                } অন্যথা

#define FOREACH_DIRENT(de, d, on_error)                                 \
        FOREACH_DIRENT_ALL(de, d, on_error)                             \
             যদি (hidden_or_backup_file((de)->d_name))                   \
                     অনুবর্ত;                                          \
             অন্যথা

/* Maximum space one dirent structure might require at most */
#define DIRENT_SIZE_MAX CONST_MAX(পরিমাণ(সংরচনা dirent), offsetof(সংরচনা dirent, d_name) + NAME_MAX + 1)

/* Only if 64-bit off_t is enabled struct dirent + struct dirent64 are actually the same. We require this, and
 * we want them to be interchangeable to make getdents64() work, hence verify that. */
assert_cc(_FILE_OFFSET_BITS == 64);
assert_cc(পরিমাণ(সংরচনা dirent) == পরিমাণ(সংরচনা dirent64));
assert_cc(offsetof(সংরচনা dirent, d_ino) == offsetof(সংরচনা dirent64, d_ino));
assert_cc(sizeof_field(সংরচনা dirent, d_ino) == sizeof_field(সংরচনা dirent64, d_ino));
assert_cc(offsetof(সংরচনা dirent, d_off) == offsetof(সংরচনা dirent64, d_off));
assert_cc(sizeof_field(সংরচনা dirent, d_off) == sizeof_field(সংরচনা dirent64, d_off));
assert_cc(offsetof(সংরচনা dirent, d_reclen) == offsetof(সংরচনা dirent64, d_reclen));
assert_cc(sizeof_field(সংরচনা dirent, d_reclen) == sizeof_field(সংরচনা dirent64, d_reclen));
assert_cc(offsetof(সংরচনা dirent, d_type) == offsetof(সংরচনা dirent64, d_type));
assert_cc(sizeof_field(সংরচনা dirent, d_type) == sizeof_field(সংরচনা dirent64, d_type));
assert_cc(offsetof(সংরচনা dirent, d_name) == offsetof(সংরচনা dirent64, d_name));
assert_cc(sizeof_field(সংরচনা dirent, d_name) == sizeof_field(সংরচনা dirent64, d_name));

#define FOREACH_DIRENT_IN_BUFFER(de, buf, sz)                           \
        প্রতি (শূন্য *_end = (uint8_t*) ({ (de) = (buf); }) + (sz);        \
             (uint8_t*) (de) < (uint8_t*) _end;                         \
             (de) = (সংরচনা dirent*) ((uint8_t*) (de) + (de)->d_reclen))

#define DEFINE_DIRENT_BUFFER(name, sz)                                  \
        সঙ্ঘ {                                                         \
                সংরচনা dirent de;                                       \
                uint8_t data[(sz) * DIRENT_SIZE_MAX];                   \
        } name
