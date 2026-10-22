/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <dirent.h>             /* IWYU pragma: export */

#அந்தர்பாவ "forward.h"
#அந்தர்பாவ "path-util.h"          /* IWYU pragma: keep */

bool dirent_is_file(மாறிலி கட்டமைப்பு dirent *de) _pure_;
bool dirent_is_file_with_suffix(மாறிலி கட்டமைப்பு dirent *de, மாறிலி எழுத்து *suffix) _pure_;
முழு dirent_ensure_type(முழு dir_fd, கட்டமைப்பு dirent *de);

கட்டமைப்பு dirent* readdir_ensure_type(DIR *d);
கட்டமைப்பு dirent* readdir_no_dot(DIR *dirp);

#define FOREACH_DIRENT_ALL(de, d, on_error)                             \
        க்கு (கட்டமைப்பு dirent *(de) = readdir_ensure_type(d);; (de) = readdir_ensure_type(d)) \
                எனில் (!de) {                                              \
                        எனில் (errno > 0) {                                \
                                on_error;                               \
                        }                                               \
                        நிறுத்து;                                          \
                } இல்லையெனில்

#define FOREACH_DIRENT(de, d, on_error)                                 \
        FOREACH_DIRENT_ALL(de, d, on_error)                             \
             எனில் (hidden_or_backup_file((de)->d_name))                   \
                     தொடர்;                                          \
             இல்லையெனில்

/* Maximum space one dirent structure might require at most */
#define DIRENT_SIZE_MAX CONST_MAX(அளவு(கட்டமைப்பு dirent), offsetof(கட்டமைப்பு dirent, d_name) + NAME_MAX + 1)

/* Only if 64-bit off_t is enabled struct dirent + struct dirent64 are actually the same. We require this, and
 * we want them to be interchangeable to make getdents64() work, hence verify that. */
assert_cc(_FILE_OFFSET_BITS == 64);
assert_cc(அளவு(கட்டமைப்பு dirent) == அளவு(கட்டமைப்பு dirent64));
assert_cc(offsetof(கட்டமைப்பு dirent, d_ino) == offsetof(கட்டமைப்பு dirent64, d_ino));
assert_cc(sizeof_field(கட்டமைப்பு dirent, d_ino) == sizeof_field(கட்டமைப்பு dirent64, d_ino));
assert_cc(offsetof(கட்டமைப்பு dirent, d_off) == offsetof(கட்டமைப்பு dirent64, d_off));
assert_cc(sizeof_field(கட்டமைப்பு dirent, d_off) == sizeof_field(கட்டமைப்பு dirent64, d_off));
assert_cc(offsetof(கட்டமைப்பு dirent, d_reclen) == offsetof(கட்டமைப்பு dirent64, d_reclen));
assert_cc(sizeof_field(கட்டமைப்பு dirent, d_reclen) == sizeof_field(கட்டமைப்பு dirent64, d_reclen));
assert_cc(offsetof(கட்டமைப்பு dirent, d_type) == offsetof(கட்டமைப்பு dirent64, d_type));
assert_cc(sizeof_field(கட்டமைப்பு dirent, d_type) == sizeof_field(கட்டமைப்பு dirent64, d_type));
assert_cc(offsetof(கட்டமைப்பு dirent, d_name) == offsetof(கட்டமைப்பு dirent64, d_name));
assert_cc(sizeof_field(கட்டமைப்பு dirent, d_name) == sizeof_field(கட்டமைப்பு dirent64, d_name));

#define FOREACH_DIRENT_IN_BUFFER(de, buf, sz)                           \
        க்கு (வெறுமை *_end = (uint8_t*) ({ (de) = (buf); }) + (sz);        \
             (uint8_t*) (de) < (uint8_t*) _end;                         \
             (de) = (கட்டமைப்பு dirent*) ((uint8_t*) (de) + (de)->d_reclen))

#define DEFINE_DIRENT_BUFFER(name, sz)                                  \
        ஒன்றிப்பு {                                                         \
                கட்டமைப்பு dirent de;                                       \
                uint8_t data[(sz) * DIRENT_SIZE_MAX];                   \
        } name
