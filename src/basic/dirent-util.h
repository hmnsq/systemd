/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <dirent.h>             /* IWYU pragma: export */

#समावेश "forward.h"
#समावेश "path-util.h"          /* IWYU pragma: keep */

bool dirent_is_file(स्थिर काष्ठा dirent *de) _pure_;
bool dirent_is_file_with_suffix(स्थिर काष्ठा dirent *de, स्थिर अक्षर *suffix) _pure_;
पूर्णांक dirent_ensure_type(पूर्णांक dir_fd, काष्ठा dirent *de);

काष्ठा dirent* readdir_ensure_type(सूची *d);
काष्ठा dirent* readdir_no_dot(सूची *dirp);

#घोषणा FOREACH_DIRENT_ALL(de, d, on_error)                             \
        क्रम (काष्ठा dirent *(de) = readdir_ensure_type(d);; (de) = readdir_ensure_type(d)) \
                अगर (!de) {                                              \
                        अगर (त्रुटि_सं > 0) {                                \
                                on_error;                               \
                        }                                               \
                        अवरोध;                                          \
                } अन्यथा

#घोषणा FOREACH_DIRENT(de, d, on_error)                                 \
        FOREACH_DIRENT_ALL(de, d, on_error)                             \
             अगर (hidden_or_backup_file((de)->d_name))                   \
                     जारी;                                          \
             अन्यथा

/* Maximum space one dirent structure might require at most */
#घोषणा DIRENT_आकार_MAX CONST_MAX(माप(काष्ठा dirent), दुरत्व(काष्ठा dirent, d_name) + NAME_MAX + 1)

/* Only if 64-bit off_t is enabled struct dirent + struct dirent64 are actually the same. We require this, and
 * we want them to be interchangeable to make getdents64() work, hence verify that. */
assert_cc(_FILE_OFFSET_BITS == 64);
assert_cc(माप(काष्ठा dirent) == माप(काष्ठा dirent64));
assert_cc(दुरत्व(काष्ठा dirent, d_ino) == दुरत्व(काष्ठा dirent64, d_ino));
assert_cc(sizeof_field(काष्ठा dirent, d_ino) == sizeof_field(काष्ठा dirent64, d_ino));
assert_cc(दुरत्व(काष्ठा dirent, d_off) == दुरत्व(काष्ठा dirent64, d_off));
assert_cc(sizeof_field(काष्ठा dirent, d_off) == sizeof_field(काष्ठा dirent64, d_off));
assert_cc(दुरत्व(काष्ठा dirent, d_reclen) == दुरत्व(काष्ठा dirent64, d_reclen));
assert_cc(sizeof_field(काष्ठा dirent, d_reclen) == sizeof_field(काष्ठा dirent64, d_reclen));
assert_cc(दुरत्व(काष्ठा dirent, d_type) == दुरत्व(काष्ठा dirent64, d_type));
assert_cc(sizeof_field(काष्ठा dirent, d_type) == sizeof_field(काष्ठा dirent64, d_type));
assert_cc(दुरत्व(काष्ठा dirent, d_name) == दुरत्व(काष्ठा dirent64, d_name));
assert_cc(sizeof_field(काष्ठा dirent, d_name) == sizeof_field(काष्ठा dirent64, d_name));

#घोषणा FOREACH_DIRENT_IN_बफर(de, buf, sz)                           \
        क्रम (व्योम *_end = (uint8_t*) ({ (de) = (buf); }) + (sz);        \
             (uint8_t*) (de) < (uint8_t*) _end;                         \
             (de) = (काष्ठा dirent*) ((uint8_t*) (de) + (de)->d_reclen))

#घोषणा DEFINE_DIRENT_बफर(name, sz)                                  \
        जोड़ {                                                         \
                काष्ठा dirent de;                                       \
                uint8_t तथ्य[(sz) * DIRENT_आकार_MAX];                   \
        } name
