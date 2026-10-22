/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <dirent.h>             /* IWYU pragma: export */

#ଅନ୍ତର୍ଭାଵ "forward.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"          /* IWYU pragma: keep */

bool dirent_is_file(ଅଚଳ ସଂରଚନା dirent *de) _pure_;
bool dirent_is_file_with_suffix(ଅଚଳ ସଂରଚନା dirent *de, ଅଚଳ ଅକ୍ଷର *suffix) _pure_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା dirent_ensure_type(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ସଂରଚନା dirent *de);

ସଂରଚନା dirent* readdir_ensure_type(DIR *d);
ସଂରଚନା dirent* readdir_no_dot(DIR *dirp);

#define FOREACH_DIRENT_ALL(de, d, on_error)                             \
        ପାଇଁ (ସଂରଚନା dirent *(de) = readdir_ensure_type(d);; (de) = readdir_ensure_type(d)) \
                ଯଦି (!de) {                                              \
                        ଯଦି (errno > 0) {                                \
                                on_error;                               \
                        }                                               \
                        ଭାଙ୍ଗ;                                          \
                } ଅନ୍ୟଥା

#define FOREACH_DIRENT(de, d, on_error)                                 \
        FOREACH_DIRENT_ALL(de, d, on_error)                             \
             ଯଦି (hidden_or_backup_file((de)->d_name))                   \
                     ଜାରି;                                          \
             ଅନ୍ୟଥା

/* Maximum space one dirent structure might require at most */
#define DIRENT_SIZE_MAX CONST_MAX(ଆକାର(ସଂରଚନା dirent), offsetof(ସଂରଚନା dirent, d_name) + NAME_MAX + 1)

/* Only if 64-bit off_t is enabled struct dirent + struct dirent64 are actually the same. We require this, and
 * we want them to be interchangeable to make getdents64() work, hence verify that. */
assert_cc(_FILE_OFFSET_BITS == 64);
assert_cc(ଆକାର(ସଂରଚନା dirent) == ଆକାର(ସଂରଚନା dirent64));
assert_cc(offsetof(ସଂରଚନା dirent, d_ino) == offsetof(ସଂରଚନା dirent64, d_ino));
assert_cc(sizeof_field(ସଂରଚନା dirent, d_ino) == sizeof_field(ସଂରଚନା dirent64, d_ino));
assert_cc(offsetof(ସଂରଚନା dirent, d_off) == offsetof(ସଂରଚନା dirent64, d_off));
assert_cc(sizeof_field(ସଂରଚନା dirent, d_off) == sizeof_field(ସଂରଚନା dirent64, d_off));
assert_cc(offsetof(ସଂରଚନା dirent, d_reclen) == offsetof(ସଂରଚନା dirent64, d_reclen));
assert_cc(sizeof_field(ସଂରଚନା dirent, d_reclen) == sizeof_field(ସଂରଚନା dirent64, d_reclen));
assert_cc(offsetof(ସଂରଚନା dirent, d_type) == offsetof(ସଂରଚନା dirent64, d_type));
assert_cc(sizeof_field(ସଂରଚନା dirent, d_type) == sizeof_field(ସଂରଚନା dirent64, d_type));
assert_cc(offsetof(ସଂରଚନା dirent, d_name) == offsetof(ସଂରଚନା dirent64, d_name));
assert_cc(sizeof_field(ସଂରଚନା dirent, d_name) == sizeof_field(ସଂରଚନା dirent64, d_name));

#define FOREACH_DIRENT_IN_BUFFER(de, buf, sz)                           \
        ପାଇଁ (ଶୂନ୍ୟ *_end = (uint8_t*) ({ (de) = (buf); }) + (sz);        \
             (uint8_t*) (de) < (uint8_t*) _end;                         \
             (de) = (ସଂରଚନା dirent*) ((uint8_t*) (de) + (de)->d_reclen))

#define DEFINE_DIRENT_BUFFER(name, sz)                                  \
        ସଂଘ {                                                         \
                ସଂରଚନା dirent de;                                       \
                uint8_t data[(sz) * DIRENT_SIZE_MAX];                   \
        } name
