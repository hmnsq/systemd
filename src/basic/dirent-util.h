/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن <dirent.h>             /* IWYU pragma: export */

#ضمن "forward.h"
#ضمن "path-util.h"          /* IWYU pragma: keep */

bool dirent_is_file(ثابت بنية dirent *de) _pure_;
bool dirent_is_file_with_suffix(ثابت بنية dirent *de, ثابت حرف *suffix) _pure_;
صحيح dirent_ensure_type(صحيح dir_fd, بنية dirent *de);

بنية dirent* readdir_ensure_type(DIR *d);
بنية dirent* readdir_no_dot(DIR *dirp);

#define FOREACH_DIRENT_ALL(de, d, on_error)                             \
        لكل (بنية dirent *(de) = readdir_ensure_type(d);; (de) = readdir_ensure_type(d)) \
                ܐܢ (!de) {                                              \
                        ܐܢ (errno > 0) {                                \
                                on_error;                               \
                        }                                               \
                        اكسر;                                          \
                } ܐܚܪܢܐ

#define FOREACH_DIRENT(de, d, on_error)                                 \
        FOREACH_DIRENT_ALL(de, d, on_error)                             \
             ܐܢ (hidden_or_backup_file((de)->d_name))                   \
                     استمر;                                          \
             ܐܚܪܢܐ

/* Maximum space one dirent structure might require at most */
#define DIRENT_SIZE_MAX CONST_MAX(حجم(بنية dirent), offsetof(بنية dirent, d_name) + NAME_MAX + 1)

/* Only if 64-bit off_t is enabled struct dirent + struct dirent64 are actually the same. We require this, and
 * we want them to be interchangeable to make getdents64() work, hence verify that. */
assert_cc(_FILE_OFFSET_BITS == 64);
assert_cc(حجم(بنية dirent) == حجم(بنية dirent64));
assert_cc(offsetof(بنية dirent, d_ino) == offsetof(بنية dirent64, d_ino));
assert_cc(sizeof_field(بنية dirent, d_ino) == sizeof_field(بنية dirent64, d_ino));
assert_cc(offsetof(بنية dirent, d_off) == offsetof(بنية dirent64, d_off));
assert_cc(sizeof_field(بنية dirent, d_off) == sizeof_field(بنية dirent64, d_off));
assert_cc(offsetof(بنية dirent, d_reclen) == offsetof(بنية dirent64, d_reclen));
assert_cc(sizeof_field(بنية dirent, d_reclen) == sizeof_field(بنية dirent64, d_reclen));
assert_cc(offsetof(بنية dirent, d_type) == offsetof(بنية dirent64, d_type));
assert_cc(sizeof_field(بنية dirent, d_type) == sizeof_field(بنية dirent64, d_type));
assert_cc(offsetof(بنية dirent, d_name) == offsetof(بنية dirent64, d_name));
assert_cc(sizeof_field(بنية dirent, d_name) == sizeof_field(بنية dirent64, d_name));

#define FOREACH_DIRENT_IN_BUFFER(de, buf, sz)                           \
        لكل (فراغ *_end = (uint8_t*) ({ (de) = (buf); }) + (sz);        \
             (uint8_t*) (de) < (uint8_t*) _end;                         \
             (de) = (بنية dirent*) ((uint8_t*) (de) + (de)->d_reclen))

#define DEFINE_DIRENT_BUFFER(name, sz)                                  \
        اتحاد {                                                         \
                بنية dirent de;                                       \
                uint8_t data[(sz) * DIRENT_SIZE_MAX];                   \
        } name
