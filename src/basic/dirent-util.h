/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <dirent.h>             /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"          /* IWYU pragma: keep */

bool dirent_is_file(ಸ್ಥಿರಾಂಕ ರಚನೆ dirent *de) ಶುದ್ಧ;
bool dirent_is_file_with_suffix(ಸ್ಥಿರಾಂಕ ರಚನೆ dirent *de, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix) ಶುದ್ಧ;
ಪೂರ್ಣಾಂಕ dirent_ensure_type(ಪೂರ್ಣಾಂಕ dir_fd, ರಚನೆ dirent *de);

ರಚನೆ dirent* readdir_ensure_type(DIR *d);
ರಚನೆ dirent* readdir_no_dot(DIR *dirp);

#ವ್ಯಾಖ್ಯಾನಿಸು FOREACH_DIRENT_ALL(de, d, on_error)                             \
        ಗಾಗಿ (ರಚನೆ dirent *(de) = readdir_ensure_type(d);; (de) = readdir_ensure_type(d)) \
                ಆದರೆ (!de) {                                              \
                        ಆದರೆ (errno > 0) {                                \
                                on_error;                               \
                        }                                               \
                        ನಿಲ್ಲಿಸು;                                          \
                } ಇಲ್ಲದಿದ್ದರೆ

#ವ್ಯಾಖ್ಯಾನಿಸು FOREACH_DIRENT(de, d, on_error)                                 \
        FOREACH_DIRENT_ALL(de, d, on_error)                             \
             ಆದರೆ (hidden_or_backup_file((de)->d_name))                   \
                     ಮುಂದುವರಿ;                                          \
             ಇಲ್ಲದಿದ್ದರೆ

/* Maximum space one dirent structure might require at most */
#ವ್ಯಾಖ್ಯಾನಿಸು DIRENT_ಗಾತ್ರ_MAX CONST_MAX(ಗಾತ್ರ(ರಚನೆ dirent), offsetof(ರಚನೆ dirent, d_name) + NAME_MAX + 1)

/* Only if 64-bit off_t is enabled struct dirent + struct dirent64 are actually the same. We require this, and
 * we want them to be interchangeable to make getdents64() work, hence verify that. */
assert_cc(FILE_ಸ್ಥಾನಾಂತರ_ಬಿಟ್ಟುಗಳು == 64);
assert_cc(ಗಾತ್ರ(ರಚನೆ dirent) == ಗಾತ್ರ(ರಚನೆ dirent64));
assert_cc(offsetof(ರಚನೆ dirent, d_ino) == offsetof(ರಚನೆ dirent64, d_ino));
assert_cc(sizeof_ಕ್ಷೇತ್ರ(ರಚನೆ dirent, d_ino) == sizeof_ಕ್ಷೇತ್ರ(ರಚನೆ dirent64, d_ino));
assert_cc(offsetof(ರಚನೆ dirent, d_off) == offsetof(ರಚನೆ dirent64, d_off));
assert_cc(sizeof_ಕ್ಷೇತ್ರ(ರಚನೆ dirent, d_off) == sizeof_ಕ್ಷೇತ್ರ(ರಚನೆ dirent64, d_off));
assert_cc(offsetof(ರಚನೆ dirent, d_reclen) == offsetof(ರಚನೆ dirent64, d_reclen));
assert_cc(sizeof_ಕ್ಷೇತ್ರ(ರಚನೆ dirent, d_reclen) == sizeof_ಕ್ಷೇತ್ರ(ರಚನೆ dirent64, d_reclen));
assert_cc(offsetof(ರಚನೆ dirent, d_type) == offsetof(ರಚನೆ dirent64, d_type));
assert_cc(sizeof_ಕ್ಷೇತ್ರ(ರಚನೆ dirent, d_type) == sizeof_ಕ್ಷೇತ್ರ(ರಚನೆ dirent64, d_type));
assert_cc(offsetof(ರಚನೆ dirent, d_name) == offsetof(ರಚನೆ dirent64, d_name));
assert_cc(sizeof_ಕ್ಷೇತ್ರ(ರಚನೆ dirent, d_name) == sizeof_ಕ್ಷೇತ್ರ(ರಚನೆ dirent64, d_name));

#ವ್ಯಾಖ್ಯಾನಿಸು FOREACH_DIRENT_ಒಳ_ಬಫರ್(de, buf, sz)                           \
        ಗಾಗಿ (ಶೂನ್ಯ *ಕೊನೆ_2 = (uint8_t*) ({ (de) = (buf); }) + (sz);        \
             (uint8_t*) (de) < (uint8_t*) ಕೊನೆ_2;                         \
             (de) = (ರಚನೆ dirent*) ((uint8_t*) (de) + (de)->d_reclen))

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_DIRENT_ಬಫರ್(name, sz)                                  \
        ಒಕ್ಕೂಟ {                                                         \
                ರಚನೆ dirent de;                                       \
                uint8_t ದತ್ತಾಂಶ[(sz) * DIRENT_ಗಾತ್ರ_MAX];                   \
        } name
