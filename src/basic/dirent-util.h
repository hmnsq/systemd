/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <dirent.h>             /* IWYU pragma: export */

#شامل "forward.h"
#شامل "path-util.h"          /* IWYU pragma: keep */

bool dirent_is_file(ثابت ساختار dirent *de) _pure_;
bool dirent_is_file_with_suffix(ثابت ساختار dirent *de, ثابت نویسه *suffix) _pure_;
صحیح dirent_ensure_type(صحیح dir_fd, ساختار dirent *de);

ساختار dirent* readdir_ensure_type(DIR *d);
ساختار dirent* readdir_no_dot(DIR *dirp);

#define FOREACH_DIRENT_ALL(de, d, on_error)                             \
        برای (ساختار dirent *(de) = readdir_ensure_type(d);; (de) = readdir_ensure_type(d)) \
                اگر (!de) {                                              \
                        اگر (errno > 0) {                                \
                                on_error;                               \
                        }                                               \
                        بشکن;                                          \
                } وگرنه

#define FOREACH_DIRENT(de, d, on_error)                                 \
        FOREACH_DIRENT_ALL(de, d, on_error)                             \
             اگر (hidden_or_backup_file((de)->d_name))                   \
                     ادامه;                                          \
             وگرنه

/* Maximum space one dirent structure might require at most */
#define DIRENT_SIZE_MAX CONST_MAX(اندازه(ساختار dirent), offsetof(ساختار dirent, d_name) + NAME_MAX + 1)

/* Only if 64-bit off_t is enabled struct dirent + struct dirent64 are actually the same. We require this, and
 * we want them to be interchangeable to make getdents64() work, hence verify that. */
assert_cc(_FILE_OFFSET_BITS == 64);
assert_cc(اندازه(ساختار dirent) == اندازه(ساختار dirent64));
assert_cc(offsetof(ساختار dirent, d_ino) == offsetof(ساختار dirent64, d_ino));
assert_cc(sizeof_field(ساختار dirent, d_ino) == sizeof_field(ساختار dirent64, d_ino));
assert_cc(offsetof(ساختار dirent, d_off) == offsetof(ساختار dirent64, d_off));
assert_cc(sizeof_field(ساختار dirent, d_off) == sizeof_field(ساختار dirent64, d_off));
assert_cc(offsetof(ساختار dirent, d_reclen) == offsetof(ساختار dirent64, d_reclen));
assert_cc(sizeof_field(ساختار dirent, d_reclen) == sizeof_field(ساختار dirent64, d_reclen));
assert_cc(offsetof(ساختار dirent, d_type) == offsetof(ساختار dirent64, d_type));
assert_cc(sizeof_field(ساختار dirent, d_type) == sizeof_field(ساختار dirent64, d_type));
assert_cc(offsetof(ساختار dirent, d_name) == offsetof(ساختار dirent64, d_name));
assert_cc(sizeof_field(ساختار dirent, d_name) == sizeof_field(ساختار dirent64, d_name));

#define FOREACH_DIRENT_IN_BUFFER(de, buf, sz)                           \
        برای (تهی *_end = (uint8_t*) ({ (de) = (buf); }) + (sz);        \
             (uint8_t*) (de) < (uint8_t*) _end;                         \
             (de) = (ساختار dirent*) ((uint8_t*) (de) + (de)->d_reclen))

#define DEFINE_DIRENT_BUFFER(name, sz)                                  \
        اجتماع {                                                         \
                ساختار dirent de;                                       \
                uint8_t data[(sz) * DIRENT_SIZE_MAX];                   \
        } name
