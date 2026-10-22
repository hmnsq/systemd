/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <dirent.h>             /* IWYU pragma: export */

#అన్తర్భావ "forward.h"
#అన్తర్భావ "path-util.h"          /* IWYU pragma: keep */

bool dirent_is_file(స్థిరరాశి నిర్మాణం dirent *de) శుద్ధ;
bool dirent_is_file_with_suffix(స్థిరరాశి నిర్మాణం dirent *de, స్థిరరాశి అక్షరం *suffix) శుద్ధ;
పూర్ణాంకం dirent_ensure_type(పూర్ణాంకం dir_fd, నిర్మాణం dirent *de);

నిర్మాణం dirent* readdir_ensure_type(DIR *d);
నిర్మాణం dirent* readdir_no_dot(DIR *dirp);

#నిర్వచించు FOREACH_DIRENT_ALL(de, d, on_error)                             \
        కొరకు (నిర్మాణం dirent *(de) = readdir_ensure_type(d);; (de) = readdir_ensure_type(d)) \
                ఒకవేళ (!de) {                                              \
                        ఒకవేళ (errno > 0) {                                \
                                on_error;                               \
                        }                                               \
                        ఆపు;                                          \
                } లేకపోతే

#నిర్వచించు FOREACH_DIRENT(de, d, on_error)                                 \
        FOREACH_DIRENT_ALL(de, d, on_error)                             \
             ఒకవేళ (hidden_or_backup_file((de)->d_name))                   \
                     కొనసాగు;                                          \
             లేకపోతే

/* Maximum space one dirent structure might require at most */
#నిర్వచించు DIRENT_కొలత_MAX CONST_MAX(పరిమాణం(నిర్మాణం dirent), offsetof(నిర్మాణం dirent, d_name) + NAME_MAX + 1)

/* Only if 64-bit off_t is enabled struct dirent + struct dirent64 are actually the same. We require this, and
 * we want them to be interchangeable to make getdents64() work, hence verify that. */
assert_cc(FILE_స్థానభ్రంశం_బిట్లు == 64);
assert_cc(పరిమాణం(నిర్మాణం dirent) == పరిమాణం(నిర్మాణం dirent64));
assert_cc(offsetof(నిర్మాణం dirent, d_ino) == offsetof(నిర్మాణం dirent64, d_ino));
assert_cc(sizeof_క్షేత్రం(నిర్మాణం dirent, d_ino) == sizeof_క్షేత్రం(నిర్మాణం dirent64, d_ino));
assert_cc(offsetof(నిర్మాణం dirent, d_off) == offsetof(నిర్మాణం dirent64, d_off));
assert_cc(sizeof_క్షేత్రం(నిర్మాణం dirent, d_off) == sizeof_క్షేత్రం(నిర్మాణం dirent64, d_off));
assert_cc(offsetof(నిర్మాణం dirent, d_reclen) == offsetof(నిర్మాణం dirent64, d_reclen));
assert_cc(sizeof_క్షేత్రం(నిర్మాణం dirent, d_reclen) == sizeof_క్షేత్రం(నిర్మాణం dirent64, d_reclen));
assert_cc(offsetof(నిర్మాణం dirent, d_type) == offsetof(నిర్మాణం dirent64, d_type));
assert_cc(sizeof_క్షేత్రం(నిర్మాణం dirent, d_type) == sizeof_క్షేత్రం(నిర్మాణం dirent64, d_type));
assert_cc(offsetof(నిర్మాణం dirent, d_name) == offsetof(నిర్మాణం dirent64, d_name));
assert_cc(sizeof_క్షేత్రం(నిర్మాణం dirent, d_name) == sizeof_క్షేత్రం(నిర్మాణం dirent64, d_name));

#నిర్వచించు FOREACH_DIRENT_లోపల_బఫరు(de, buf, sz)                           \
        కొరకు (శూన్య *చివర_2 = (uint8_t*) ({ (de) = (buf); }) + (sz);        \
             (uint8_t*) (de) < (uint8_t*) చివర_2;                         \
             (de) = (నిర్మాణం dirent*) ((uint8_t*) (de) + (de)->d_reclen))

#నిర్వచించు DEFINE_DIRENT_బఫరు(name, sz)                                  \
        సంఘం {                                                         \
                నిర్మాణం dirent de;                                       \
                uint8_t దత్తాంశం[(sz) * DIRENT_కొలత_MAX];                   \
        } name
