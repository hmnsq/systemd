/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <dirent.h>             /* IWYU pragma: export */

#അന്തര്ഭാവ "forward.h"
#അന്തര്ഭാവ "path-util.h"          /* IWYU pragma: keep */

bool dirent_is_file(സ്ഥിരം ഘടന dirent *de) _pure_;
bool dirent_is_file_with_suffix(സ്ഥിരം ഘടന dirent *de, സ്ഥിരം അക്ഷരം *suffix) _pure_;
പൂർണ്ണസംഖ്യ dirent_ensure_type(പൂർണ്ണസംഖ്യ dir_fd, ഘടന dirent *de);

ഘടന dirent* readdir_ensure_type(DIR *d);
ഘടന dirent* readdir_no_dot(DIR *dirp);

#define FOREACH_DIRENT_ALL(de, d, on_error)                             \
        വേണ്ടി (ഘടന dirent *(de) = readdir_ensure_type(d);; (de) = readdir_ensure_type(d)) \
                എങ്കിൽ (!de) {                                              \
                        എങ്കിൽ (errno > 0) {                                \
                                on_error;                               \
                        }                                               \
                        നിർത്തുക;                                          \
                } അല്ലെങ്കിൽ

#define FOREACH_DIRENT(de, d, on_error)                                 \
        FOREACH_DIRENT_ALL(de, d, on_error)                             \
             എങ്കിൽ (hidden_or_backup_file((de)->d_name))                   \
                     തുടരുക;                                          \
             അല്ലെങ്കിൽ

/* Maximum space one dirent structure might require at most */
#define DIRENT_SIZE_MAX CONST_MAX(വലുപ്പം(ഘടന dirent), offsetof(ഘടന dirent, d_name) + NAME_MAX + 1)

/* Only if 64-bit off_t is enabled struct dirent + struct dirent64 are actually the same. We require this, and
 * we want them to be interchangeable to make getdents64() work, hence verify that. */
assert_cc(_FILE_OFFSET_BITS == 64);
assert_cc(വലുപ്പം(ഘടന dirent) == വലുപ്പം(ഘടന dirent64));
assert_cc(offsetof(ഘടന dirent, d_ino) == offsetof(ഘടന dirent64, d_ino));
assert_cc(sizeof_field(ഘടന dirent, d_ino) == sizeof_field(ഘടന dirent64, d_ino));
assert_cc(offsetof(ഘടന dirent, d_off) == offsetof(ഘടന dirent64, d_off));
assert_cc(sizeof_field(ഘടന dirent, d_off) == sizeof_field(ഘടന dirent64, d_off));
assert_cc(offsetof(ഘടന dirent, d_reclen) == offsetof(ഘടന dirent64, d_reclen));
assert_cc(sizeof_field(ഘടന dirent, d_reclen) == sizeof_field(ഘടന dirent64, d_reclen));
assert_cc(offsetof(ഘടന dirent, d_type) == offsetof(ഘടന dirent64, d_type));
assert_cc(sizeof_field(ഘടന dirent, d_type) == sizeof_field(ഘടന dirent64, d_type));
assert_cc(offsetof(ഘടന dirent, d_name) == offsetof(ഘടന dirent64, d_name));
assert_cc(sizeof_field(ഘടന dirent, d_name) == sizeof_field(ഘടന dirent64, d_name));

#define FOREACH_DIRENT_IN_BUFFER(de, buf, sz)                           \
        വേണ്ടി (ശൂന്യം *_end = (uint8_t*) ({ (de) = (buf); }) + (sz);        \
             (uint8_t*) (de) < (uint8_t*) _end;                         \
             (de) = (ഘടന dirent*) ((uint8_t*) (de) + (de)->d_reclen))

#define DEFINE_DIRENT_BUFFER(name, sz)                                  \
        സംഘം {                                                         \
                ഘടന dirent de;                                       \
                uint8_t data[(sz) * DIRENT_SIZE_MAX];                   \
        } name
