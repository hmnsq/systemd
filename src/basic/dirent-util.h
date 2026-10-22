/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <dirent.h>             /* IWYU pragma: export */

#अन्तर्भाव "forward.h"
#अन्तर्भाव "path-util.h"          /* IWYU pragma: keep */

bool dirent_is_file(ध्रुव संरचना dirent *de) विशुद्धम्;
bool dirent_is_file_with_suffix(ध्रुव संरचना dirent *de, ध्रुव वर्ण *suffix) विशुद्धम्;
पूर्णाङ्क dirent_ensure_type(पूर्णाङ्क dir_fd, संरचना dirent *de);

संरचना dirent* readdir_ensure_type(DIR *d);
संरचना dirent* readdir_no_dot(DIR *dirp);

#परिभाषय FOREACH_DIRENT_ALL(de, d, on_error)                             \
        प्रति (संरचना dirent *(de) = readdir_ensure_type(d);; (de) = readdir_ensure_type(d)) \
                यदि (!de) {                                              \
                        यदि (errno > 0) {                                \
                                on_error;                               \
                        }                                               \
                        भङ्ग;                                          \
                } अन्यथा

#परिभाषय FOREACH_DIRENT(de, d, on_error)                                 \
        FOREACH_DIRENT_ALL(de, d, on_error)                             \
             यदि (hidden_or_backup_file((de)->d_name))                   \
                     अनुवर्त;                                          \
             अन्यथा

/* Maximum space one dirent structure might require at most */
#परिभाषय DIRENT_आकार_MAX CONST_MAX(परिमाण(संरचना dirent), offsetof(संरचना dirent, d_name) + NAME_MAX + 1)

/* Only if 64-bit off_t is enabled struct dirent + struct dirent64 are actually the same. We require this, and
 * we want them to be interchangeable to make getdents64() work, hence verify that. */
assert_cc(FILE_विस्थापनम्_द्वयङ्काः == 64);
assert_cc(परिमाण(संरचना dirent) == परिमाण(संरचना dirent64));
assert_cc(offsetof(संरचना dirent, d_ino) == offsetof(संरचना dirent64, d_ino));
assert_cc(sizeof_क्षेत्रम्(संरचना dirent, d_ino) == sizeof_क्षेत्रम्(संरचना dirent64, d_ino));
assert_cc(offsetof(संरचना dirent, d_off) == offsetof(संरचना dirent64, d_off));
assert_cc(sizeof_क्षेत्रम्(संरचना dirent, d_off) == sizeof_क्षेत्रम्(संरचना dirent64, d_off));
assert_cc(offsetof(संरचना dirent, d_reclen) == offsetof(संरचना dirent64, d_reclen));
assert_cc(sizeof_क्षेत्रम्(संरचना dirent, d_reclen) == sizeof_क्षेत्रम्(संरचना dirent64, d_reclen));
assert_cc(offsetof(संरचना dirent, d_type) == offsetof(संरचना dirent64, d_type));
assert_cc(sizeof_क्षेत्रम्(संरचना dirent, d_type) == sizeof_क्षेत्रम्(संरचना dirent64, d_type));
assert_cc(offsetof(संरचना dirent, d_name) == offsetof(संरचना dirent64, d_name));
assert_cc(sizeof_क्षेत्रम्(संरचना dirent, d_name) == sizeof_क्षेत्रम्(संरचना dirent64, d_name));

#परिभाषय FOREACH_DIRENT_अन्तर्_बफर(de, buf, sz)                           \
        प्रति (शून्य *अन्तः_2 = (uint8_t*) ({ (de) = (buf); }) + (sz);        \
             (uint8_t*) (de) < (uint8_t*) अन्तः_2;                         \
             (de) = (संरचना dirent*) ((uint8_t*) (de) + (de)->d_reclen))

#परिभाषय DEFINE_DIRENT_बफर(name, sz)                                  \
        सङ्घ {                                                         \
                संरचना dirent de;                                       \
                uint8_t दत्त[(sz) * DIRENT_आकार_MAX];                   \
        } name
