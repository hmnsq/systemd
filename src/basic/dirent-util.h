/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن <dirent.h>             /* IWYU pragma: export */

#ضمن "forward.h"
#ضمن "path-util.h"          /* IWYU pragma: keep */

bool dirent_is_file(ثابت بنية dirent *de) خالص;
bool dirent_is_file_with_suffix(ثابت بنية dirent *de, ثابت حرف *suffix) خالص;
صحيح dirent_ensure_type(صحيح dir_fd, بنية dirent *de);

بنية dirent* readdir_ensure_type(DIR *d);
بنية dirent* readdir_no_dot(DIR *dirp);

#تعریف_کن FOREACH_DIRENT_ALL(de, d, on_error)                             \
        لكل (بنية dirent *(de) = readdir_ensure_type(d);; (de) = readdir_ensure_type(d)) \
                إذا (!de) {                                              \
                        إذا (errno > 0) {                                \
                                on_error;                               \
                        }                                               \
                        اكسر;                                          \
                } وإلا

#تعریف_کن FOREACH_DIRENT(de, d, on_error)                                 \
        FOREACH_DIRENT_ALL(de, d, on_error)                             \
             إذا (hidden_or_backup_file((de)->d_name))                   \
                     استمر;                                          \
             وإلا

/* Maximum space one dirent structure might require at most */
#تعریف_کن DIRENT_اندازه_MAX CONST_MAX(حجم(بنية dirent), offsetof(بنية dirent, d_name) + NAME_MAX + 1)

/* Only if 64-bit off_t is enabled struct dirent + struct dirent64 are actually the same. We require this, and
 * we want them to be interchangeable to make getdents64() work, hence verify that. */
assert_cc(FILE_جابه_جایی_بیتها == 64);
assert_cc(حجم(بنية dirent) == حجم(بنية dirent64));
assert_cc(offsetof(بنية dirent, d_ino) == offsetof(بنية dirent64, d_ino));
assert_cc(sizeof_فیلد(بنية dirent, d_ino) == sizeof_فیلد(بنية dirent64, d_ino));
assert_cc(offsetof(بنية dirent, d_off) == offsetof(بنية dirent64, d_off));
assert_cc(sizeof_فیلد(بنية dirent, d_off) == sizeof_فیلد(بنية dirent64, d_off));
assert_cc(offsetof(بنية dirent, d_reclen) == offsetof(بنية dirent64, d_reclen));
assert_cc(sizeof_فیلد(بنية dirent, d_reclen) == sizeof_فیلد(بنية dirent64, d_reclen));
assert_cc(offsetof(بنية dirent, d_type) == offsetof(بنية dirent64, d_type));
assert_cc(sizeof_فیلد(بنية dirent, d_type) == sizeof_فیلد(بنية dirent64, d_type));
assert_cc(offsetof(بنية dirent, d_name) == offsetof(بنية dirent64, d_name));
assert_cc(sizeof_فیلد(بنية dirent, d_name) == sizeof_فیلد(بنية dirent64, d_name));

#تعریف_کن FOREACH_DIRENT_درون_میانگیر(de, buf, sz)                           \
        لكل (فراغ *انتها_2 = (uint8_t*) ({ (de) = (buf); }) + (sz);        \
             (uint8_t*) (de) < (uint8_t*) انتها_2;                         \
             (de) = (بنية dirent*) ((uint8_t*) (de) + (de)->d_reclen))

#تعریف_کن DEFINE_DIRENT_میانگیر(name, sz)                                  \
        اتحاد {                                                         \
                بنية dirent de;                                       \
                uint8_t داده[(sz) * DIRENT_اندازه_MAX];                   \
        } name
