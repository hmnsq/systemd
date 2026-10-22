/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <dirent.h>             /* IWYU pragma: export */

#شامل "forward.h"
#شامل "path-util.h"          /* IWYU pragma: keep */

bool dirent_is_file(ثابت ساخت dirent *de) خالص;
bool dirent_is_file_with_suffix(ثابت ساخت dirent *de, ثابت حرف *suffix) خالص;
صحیح dirent_ensure_type(صحیح dir_fd, ساخت dirent *de);

ساخت dirent* readdir_ensure_type(DIR *d);
ساخت dirent* readdir_no_dot(DIR *dirp);

#تعریف_کرو FOREACH_DIRENT_ALL(de, d, on_error)                             \
        برائے (ساخت dirent *(de) = readdir_ensure_type(d);; (de) = readdir_ensure_type(d)) \
                اگر (!de) {                                              \
                        اگر (errno > 0) {                                \
                                on_error;                               \
                        }                                               \
                        توڑو;                                          \
                } ورنہ

#تعریف_کرو FOREACH_DIRENT(de, d, on_error)                                 \
        FOREACH_DIRENT_ALL(de, d, on_error)                             \
             اگر (hidden_or_backup_file((de)->d_name))                   \
                     جاری;                                          \
             ورنہ

/* Maximum space one dirent structure might require at most */
#تعریف_کرو DIRENT_سائز_MAX CONST_MAX(حجم(ساخت dirent), offsetof(ساخت dirent, d_name) + NAME_MAX + 1)

/* Only if 64-bit off_t is enabled struct dirent + struct dirent64 are actually the same. We require this, and
 * we want them to be interchangeable to make getdents64() work, hence verify that. */
assert_cc(FILE_انحراف_بٹ == 64);
assert_cc(حجم(ساخت dirent) == حجم(ساخت dirent64));
assert_cc(offsetof(ساخت dirent, d_ino) == offsetof(ساخت dirent64, d_ino));
assert_cc(sizeof_میدان(ساخت dirent, d_ino) == sizeof_میدان(ساخت dirent64, d_ino));
assert_cc(offsetof(ساخت dirent, d_off) == offsetof(ساخت dirent64, d_off));
assert_cc(sizeof_میدان(ساخت dirent, d_off) == sizeof_میدان(ساخت dirent64, d_off));
assert_cc(offsetof(ساخت dirent, d_reclen) == offsetof(ساخت dirent64, d_reclen));
assert_cc(sizeof_میدان(ساخت dirent, d_reclen) == sizeof_میدان(ساخت dirent64, d_reclen));
assert_cc(offsetof(ساخت dirent, d_type) == offsetof(ساخت dirent64, d_type));
assert_cc(sizeof_میدان(ساخت dirent, d_type) == sizeof_میدان(ساخت dirent64, d_type));
assert_cc(offsetof(ساخت dirent, d_name) == offsetof(ساخت dirent64, d_name));
assert_cc(sizeof_میدان(ساخت dirent, d_name) == sizeof_میدان(ساخت dirent64, d_name));

#تعریف_کرو FOREACH_DIRENT_اندر_بفر(de, buf, sz)                           \
        برائے (خالی *انتہا_2 = (uint8_t*) ({ (de) = (buf); }) + (sz);        \
             (uint8_t*) (de) < (uint8_t*) انتہا_2;                         \
             (de) = (ساخت dirent*) ((uint8_t*) (de) + (de)->d_reclen))

#تعریف_کرو DEFINE_DIRENT_بفر(name, sz)                                  \
        اتحاد {                                                         \
                ساخت dirent de;                                       \
                uint8_t ڈیٹا[(sz) * DIRENT_سائز_MAX];                   \
        } name
