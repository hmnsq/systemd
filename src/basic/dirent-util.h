/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <dirent.h>             /* IWYU pragma: export */

#include "forward.h"
#include "path-util.h"          /* IWYU pragma: keep */

bool dirent_is_file(const sa_nracanaa dirent *de) _pure_;
bool dirent_is_file_with_suffix(const sa_nracanaa dirent *de, const var_na *suffix) _pure_;
ຈຳນວນເຕັມ dirent_ensure_type(ຈຳນວນເຕັມ dir_fd, sa_nracanaa dirent *de);

sa_nracanaa dirent* readdir_ensure_type(DIR *d);
sa_nracanaa dirent* readdir_no_dot(DIR *dirp);

#define FOREACH_DIRENT_ALL(de, d, on_error)                             \
        ສຳລັບ (sa_nracanaa dirent *(de) = readdir_ensure_type(d);; (de) = readdir_ensure_type(d)) \
                ຖ້າ (!de) {                                              \
                        ຖ້າ (errno > 0) {                                \
                                on_error;                               \
                        }                                               \
                        bha_nga;                                          \
                } ບໍ່ດັ່ງນັ້ນ

#define FOREACH_DIRENT(de, d, on_error)                                 \
        FOREACH_DIRENT_ALL(de, d, on_error)                             \
             ຖ້າ (hidden_or_backup_file((de)->d_name))                   \
                     continue;                                          \
             ບໍ່ດັ່ງນັ້ນ

/* Maximum space one dirent structure might require at most */
#define DIRENT_SIZE_MAX CONST_MAX(parimaa_na(sa_nracanaa dirent), offsetof(sa_nracanaa dirent, d_name) + NAME_MAX + 1)

/* Only if 64-bit off_t is enabled struct dirent + struct dirent64 are actually the same. We require this, and
 * we want them to be interchangeable to make getdents64() work, hence verify that. */
assert_cc(_FILE_OFFSET_BITS == 64);
assert_cc(parimaa_na(sa_nracanaa dirent) == parimaa_na(sa_nracanaa dirent64));
assert_cc(offsetof(sa_nracanaa dirent, d_ino) == offsetof(sa_nracanaa dirent64, d_ino));
assert_cc(sizeof_field(sa_nracanaa dirent, d_ino) == sizeof_field(sa_nracanaa dirent64, d_ino));
assert_cc(offsetof(sa_nracanaa dirent, d_off) == offsetof(sa_nracanaa dirent64, d_off));
assert_cc(sizeof_field(sa_nracanaa dirent, d_off) == sizeof_field(sa_nracanaa dirent64, d_off));
assert_cc(offsetof(sa_nracanaa dirent, d_reclen) == offsetof(sa_nracanaa dirent64, d_reclen));
assert_cc(sizeof_field(sa_nracanaa dirent, d_reclen) == sizeof_field(sa_nracanaa dirent64, d_reclen));
assert_cc(offsetof(sa_nracanaa dirent, d_type) == offsetof(sa_nracanaa dirent64, d_type));
assert_cc(sizeof_field(sa_nracanaa dirent, d_type) == sizeof_field(sa_nracanaa dirent64, d_type));
assert_cc(offsetof(sa_nracanaa dirent, d_name) == offsetof(sa_nracanaa dirent64, d_name));
assert_cc(sizeof_field(sa_nracanaa dirent, d_name) == sizeof_field(sa_nracanaa dirent64, d_name));

#define FOREACH_DIRENT_IN_BUFFER(de, buf, sz)                           \
        ສຳລັບ (void *_end = (uint8_t*) ({ (de) = (buf); }) + (sz);        \
             (uint8_t*) (de) < (uint8_t*) _end;                         \
             (de) = (sa_nracanaa dirent*) ((uint8_t*) (de) + (de)->d_reclen))

#define DEFINE_DIRENT_BUFFER(name, sz)                                  \
        sa_ngha {                                                         \
                sa_nracanaa dirent de;                                       \
                uint8_t data[(sz) * DIRENT_SIZE_MAX];                   \
        } name
