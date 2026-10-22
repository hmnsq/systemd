/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <dirent.h>             /* IWYU pragma: export */

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"          /* IWYU pragma: keep */

bool dirent_is_file(ਅਚਲ ਬਣਤਰ dirent *de) ਸ਼ੁੱਧ;
bool dirent_is_file_with_suffix(ਅਚਲ ਬਣਤਰ dirent *de, ਅਚਲ ਅੱਖਰ *suffix) ਸ਼ੁੱਧ;
ਪੂਰਨ dirent_ensure_type(ਪੂਰਨ dir_fd, ਬਣਤਰ dirent *de);

ਬਣਤਰ dirent* readdir_ensure_type(DIR *d);
ਬਣਤਰ dirent* readdir_no_dot(DIR *dirp);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOREACH_DIRENT_ALL(de, d, on_error)                             \
        ਲਈ (ਬਣਤਰ dirent *(de) = readdir_ensure_type(d);; (de) = readdir_ensure_type(d)) \
                ਜੇ (!de) {                                              \
                        ਜੇ (errno > 0) {                                \
                                on_error;                               \
                        }                                               \
                        ਰੋਕੋ;                                          \
                } ਨਹੀਂਤਾਂ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOREACH_DIRENT(de, d, on_error)                                 \
        FOREACH_DIRENT_ALL(de, d, on_error)                             \
             ਜੇ (hidden_or_backup_file((de)->d_name))                   \
                     ਜਾਰੀ;                                          \
             ਨਹੀਂਤਾਂ

/* Maximum space one dirent structure might require at most */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIRENT_ਮਾਪ_MAX CONST_MAX(ਆਕਾਰ(ਬਣਤਰ dirent), offsetof(ਬਣਤਰ dirent, d_name) + NAME_MAX + 1)

/* Only if 64-bit off_t is enabled struct dirent + struct dirent64 are actually the same. We require this, and
 * we want them to be interchangeable to make getdents64() work, hence verify that. */
assert_cc(FILE_ਵਿੱਥ_ਬਿੱਟ == 64);
assert_cc(ਆਕਾਰ(ਬਣਤਰ dirent) == ਆਕਾਰ(ਬਣਤਰ dirent64));
assert_cc(offsetof(ਬਣਤਰ dirent, d_ino) == offsetof(ਬਣਤਰ dirent64, d_ino));
assert_cc(sizeof_ਖੇਤਰ(ਬਣਤਰ dirent, d_ino) == sizeof_ਖੇਤਰ(ਬਣਤਰ dirent64, d_ino));
assert_cc(offsetof(ਬਣਤਰ dirent, d_off) == offsetof(ਬਣਤਰ dirent64, d_off));
assert_cc(sizeof_ਖੇਤਰ(ਬਣਤਰ dirent, d_off) == sizeof_ਖੇਤਰ(ਬਣਤਰ dirent64, d_off));
assert_cc(offsetof(ਬਣਤਰ dirent, d_reclen) == offsetof(ਬਣਤਰ dirent64, d_reclen));
assert_cc(sizeof_ਖੇਤਰ(ਬਣਤਰ dirent, d_reclen) == sizeof_ਖੇਤਰ(ਬਣਤਰ dirent64, d_reclen));
assert_cc(offsetof(ਬਣਤਰ dirent, d_type) == offsetof(ਬਣਤਰ dirent64, d_type));
assert_cc(sizeof_ਖੇਤਰ(ਬਣਤਰ dirent, d_type) == sizeof_ਖੇਤਰ(ਬਣਤਰ dirent64, d_type));
assert_cc(offsetof(ਬਣਤਰ dirent, d_name) == offsetof(ਬਣਤਰ dirent64, d_name));
assert_cc(sizeof_ਖੇਤਰ(ਬਣਤਰ dirent, d_name) == sizeof_ਖੇਤਰ(ਬਣਤਰ dirent64, d_name));

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOREACH_DIRENT_ਵਿੱਚ_ਬਫਰ(de, buf, sz)                           \
        ਲਈ (ਖਾਲੀ *ਸਿਰਾ_2 = (uint8_t*) ({ (de) = (buf); }) + (sz);        \
             (uint8_t*) (de) < (uint8_t*) ਸਿਰਾ_2;                         \
             (de) = (ਬਣਤਰ dirent*) ((uint8_t*) (de) + (de)->d_reclen))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_DIRENT_ਬਫਰ(name, sz)                                  \
        ਸੰਘ {                                                         \
                ਬਣਤਰ dirent de;                                       \
                uint8_t ਅੰਕੜੇ[(sz) * DIRENT_ਮਾਪ_MAX];                   \
        } name
