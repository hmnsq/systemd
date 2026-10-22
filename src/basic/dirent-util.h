/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <dirent.h>             /* IWYU pragma: export */

#अन्तर्भाव "forward.h"
#अन्तर्भाव "path-util.h"          /* IWYU pragma: keep */

bool dirent_is_file(स्थिरांक संरचना dirent *de) _pure_;
bool dirent_is_file_with_suffix(स्थिरांक संरचना dirent *de, स्थिरांक अक्षर *suffix) _pure_;
पूर्णांक dirent_ensure_type(पूर्णांक dir_fd, संरचना dirent *de);

संरचना dirent* readdir_ensure_type(सूची *d);
संरचना dirent* readdir_no_dot(सूची *dirp);

#define FOREACH_DIRENT_ALL(de, d, on_error)                             \
        साठी (संरचना dirent *(de) = readdir_ensure_type(d);; (de) = readdir_ensure_type(d)) \
                जर (!de) {                                              \
                        जर (त्रुटि_सं > 0) {                                \
                                on_error;                               \
                        }                                               \
                        थांबा;                                          \
                } अन्यथा

#define FOREACH_DIRENT(de, d, on_error)                                 \
        FOREACH_DIRENT_ALL(de, d, on_error)                             \
             जर (hidden_or_backup_file((de)->d_name))                   \
                     सुरूठेवा;                                          \
             अन्यथा

/* Maximum space one dirent structure might require at most */
#define DIRENT_SIZE_MAX CONST_MAX(आकार(संरचना dirent), दुरत्व(संरचना dirent, d_name) + NAME_MAX + 1)

/* Only if 64-bit off_t is enabled struct dirent + struct dirent64 are actually the same. We require this, and
 * we want them to be interchangeable to make getdents64() work, hence verify that. */
assert_cc(_FILE_OFFSET_BITS == 64);
assert_cc(आकार(संरचना dirent) == आकार(संरचना dirent64));
assert_cc(दुरत्व(संरचना dirent, d_ino) == दुरत्व(संरचना dirent64, d_ino));
assert_cc(sizeof_field(संरचना dirent, d_ino) == sizeof_field(संरचना dirent64, d_ino));
assert_cc(दुरत्व(संरचना dirent, d_off) == दुरत्व(संरचना dirent64, d_off));
assert_cc(sizeof_field(संरचना dirent, d_off) == sizeof_field(संरचना dirent64, d_off));
assert_cc(दुरत्व(संरचना dirent, d_reclen) == दुरत्व(संरचना dirent64, d_reclen));
assert_cc(sizeof_field(संरचना dirent, d_reclen) == sizeof_field(संरचना dirent64, d_reclen));
assert_cc(दुरत्व(संरचना dirent, d_type) == दुरत्व(संरचना dirent64, d_type));
assert_cc(sizeof_field(संरचना dirent, d_type) == sizeof_field(संरचना dirent64, d_type));
assert_cc(दुरत्व(संरचना dirent, d_name) == दुरत्व(संरचना dirent64, d_name));
assert_cc(sizeof_field(संरचना dirent, d_name) == sizeof_field(संरचना dirent64, d_name));

#define FOREACH_DIRENT_IN_BUFFER(de, buf, sz)                           \
        साठी (शून्य *_end = (uint8_t*) ({ (de) = (buf); }) + (sz);        \
             (uint8_t*) (de) < (uint8_t*) _end;                         \
             (de) = (संरचना dirent*) ((uint8_t*) (de) + (de)->d_reclen))

#define DEFINE_DIRENT_BUFFER(name, sz)                                  \
        संघ {                                                         \
                संरचना dirent de;                                       \
                uint8_t data[(sz) * DIRENT_SIZE_MAX];                   \
        } name
