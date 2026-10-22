/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <dirent.h>             /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"          /* IWYU pragma: keep */

bool dirent_is_file(ಧ್ರುವ ಸಂರಚನಾ dirent *de) _pure_;
bool dirent_is_file_with_suffix(ಧ್ರುವ ಸಂರಚನಾ dirent *de, ಧ್ರುವ ವರ್ಣ *suffix) _pure_;
ಪೂರ್ಣಾಙ್ಕ dirent_ensure_type(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಸಂರಚನಾ dirent *de);

ಸಂರಚನಾ dirent* readdir_ensure_type(DIR *d);
ಸಂರಚನಾ dirent* readdir_no_dot(DIR *dirp);

#define FOREACH_DIRENT_ALL(de, d, on_error)                             \
        ಪ್ರತಿ (ಸಂರಚನಾ dirent *(de) = readdir_ensure_type(d);; (de) = readdir_ensure_type(d)) \
                ಯದಿ (!de) {                                              \
                        ಯದಿ (errno > 0) {                                \
                                on_error;                               \
                        }                                               \
                        ಭಙ್ಗ;                                          \
                } ಅನ್ಯಥಾ

#define FOREACH_DIRENT(de, d, on_error)                                 \
        FOREACH_DIRENT_ALL(de, d, on_error)                             \
             ಯದಿ (hidden_or_backup_file((de)->d_name))                   \
                     ಅನುವರ್ತ;                                          \
             ಅನ್ಯಥಾ

/* Maximum space one dirent structure might require at most */
#define DIRENT_SIZE_MAX CONST_MAX(ಪರಿಮಾಣ(ಸಂರಚನಾ dirent), offsetof(ಸಂರಚನಾ dirent, d_name) + NAME_MAX + 1)

/* Only if 64-bit off_t is enabled struct dirent + struct dirent64 are actually the same. We require this, and
 * we want them to be interchangeable to make getdents64() work, hence verify that. */
assert_cc(_FILE_OFFSET_BITS == 64);
assert_cc(ಪರಿಮಾಣ(ಸಂರಚನಾ dirent) == ಪರಿಮಾಣ(ಸಂರಚನಾ dirent64));
assert_cc(offsetof(ಸಂರಚನಾ dirent, d_ino) == offsetof(ಸಂರಚನಾ dirent64, d_ino));
assert_cc(sizeof_field(ಸಂರಚನಾ dirent, d_ino) == sizeof_field(ಸಂರಚನಾ dirent64, d_ino));
assert_cc(offsetof(ಸಂರಚನಾ dirent, d_off) == offsetof(ಸಂರಚನಾ dirent64, d_off));
assert_cc(sizeof_field(ಸಂರಚನಾ dirent, d_off) == sizeof_field(ಸಂರಚನಾ dirent64, d_off));
assert_cc(offsetof(ಸಂರಚನಾ dirent, d_reclen) == offsetof(ಸಂರಚನಾ dirent64, d_reclen));
assert_cc(sizeof_field(ಸಂರಚನಾ dirent, d_reclen) == sizeof_field(ಸಂರಚನಾ dirent64, d_reclen));
assert_cc(offsetof(ಸಂರಚನಾ dirent, d_type) == offsetof(ಸಂರಚನಾ dirent64, d_type));
assert_cc(sizeof_field(ಸಂರಚನಾ dirent, d_type) == sizeof_field(ಸಂರಚನಾ dirent64, d_type));
assert_cc(offsetof(ಸಂರಚನಾ dirent, d_name) == offsetof(ಸಂರಚನಾ dirent64, d_name));
assert_cc(sizeof_field(ಸಂರಚನಾ dirent, d_name) == sizeof_field(ಸಂರಚನಾ dirent64, d_name));

#define FOREACH_DIRENT_IN_BUFFER(de, buf, sz)                           \
        ಪ್ರತಿ (ಶೂನ್ಯ *_end = (uint8_t*) ({ (de) = (buf); }) + (sz);        \
             (uint8_t*) (de) < (uint8_t*) _end;                         \
             (de) = (ಸಂರಚನಾ dirent*) ((uint8_t*) (de) + (de)->d_reclen))

#define DEFINE_DIRENT_BUFFER(name, sz)                                  \
        ಸಙ್ಘ {                                                         \
                ಸಂರಚನಾ dirent de;                                       \
                uint8_t data[(sz) * DIRENT_SIZE_MAX];                   \
        } name
