/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <dirent.h>             /* IWYU pragma: export */

#અન્તર્ભાવ "forward.h"
#અન્તર્ભાવ "path-util.h"          /* IWYU pragma: keep */

bool dirent_is_file(અચલ સંરચના dirent *de) _pure_;
bool dirent_is_file_with_suffix(અચલ સંરચના dirent *de, અચલ અક્ષર *suffix) _pure_;
પૂર્ણાંક dirent_ensure_type(પૂર્ણાંક dir_fd, સંરચના dirent *de);

સંરચના dirent* readdir_ensure_type(DIR *d);
સંરચના dirent* readdir_no_dot(DIR *dirp);

#define FOREACH_DIRENT_ALL(de, d, on_error)                             \
        માટે (સંરચના dirent *(de) = readdir_ensure_type(d);; (de) = readdir_ensure_type(d)) \
                જો (!de) {                                              \
                        જો (errno > 0) {                                \
                                on_error;                               \
                        }                                               \
                        તોડો;                                          \
                } નહીંતર

#define FOREACH_DIRENT(de, d, on_error)                                 \
        FOREACH_DIRENT_ALL(de, d, on_error)                             \
             જો (hidden_or_backup_file((de)->d_name))                   \
                     ચાલુ;                                          \
             નહીંતર

/* Maximum space one dirent structure might require at most */
#define DIRENT_SIZE_MAX CONST_MAX(કદ(સંરચના dirent), offsetof(સંરચના dirent, d_name) + NAME_MAX + 1)

/* Only if 64-bit off_t is enabled struct dirent + struct dirent64 are actually the same. We require this, and
 * we want them to be interchangeable to make getdents64() work, hence verify that. */
assert_cc(_FILE_OFFSET_BITS == 64);
assert_cc(કદ(સંરચના dirent) == કદ(સંરચના dirent64));
assert_cc(offsetof(સંરચના dirent, d_ino) == offsetof(સંરચના dirent64, d_ino));
assert_cc(sizeof_field(સંરચના dirent, d_ino) == sizeof_field(સંરચના dirent64, d_ino));
assert_cc(offsetof(સંરચના dirent, d_off) == offsetof(સંરચના dirent64, d_off));
assert_cc(sizeof_field(સંરચના dirent, d_off) == sizeof_field(સંરચના dirent64, d_off));
assert_cc(offsetof(સંરચના dirent, d_reclen) == offsetof(સંરચના dirent64, d_reclen));
assert_cc(sizeof_field(સંરચના dirent, d_reclen) == sizeof_field(સંરચના dirent64, d_reclen));
assert_cc(offsetof(સંરચના dirent, d_type) == offsetof(સંરચના dirent64, d_type));
assert_cc(sizeof_field(સંરચના dirent, d_type) == sizeof_field(સંરચના dirent64, d_type));
assert_cc(offsetof(સંરચના dirent, d_name) == offsetof(સંરચના dirent64, d_name));
assert_cc(sizeof_field(સંરચના dirent, d_name) == sizeof_field(સંરચના dirent64, d_name));

#define FOREACH_DIRENT_IN_BUFFER(de, buf, sz)                           \
        માટે (શૂન્ય *_end = (uint8_t*) ({ (de) = (buf); }) + (sz);        \
             (uint8_t*) (de) < (uint8_t*) _end;                         \
             (de) = (સંરચના dirent*) ((uint8_t*) (de) + (de)->d_reclen))

#define DEFINE_DIRENT_BUFFER(name, sz)                                  \
        સંઘ {                                                         \
                સંરચના dirent de;                                       \
                uint8_t data[(sz) * DIRENT_SIZE_MAX];                   \
        } name
