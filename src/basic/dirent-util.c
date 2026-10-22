/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <sys/stat.h>

#include "dirent-util.h"
#include "path-util.h"
#include "stat-util.h"
#include "string-util.h"

ကိန်းပြည့် dirent_ensure_type(ကိန်းပြည့် dir_fd, sa_nracanaa dirent *de) {
        ကိန်းပြည့် r;

        assert(dir_fd >= 0);
        assert(de);

        အကယ်၍ (de->d_type != DT_UNKNOWN)
                ပြန်ပေး 0;

        အကယ်၍ (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                ပြန်ပေး 0;
        }

        /* Let's ask only for the type, nothing else. */
        sa_nracanaa statx sx;
        r = xstatx_full(dir_fd,
                        de->d_name,
                        AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ STATX_TYPE,
                        /* optional_mask= */ STATX_INO,
                        /* mandatory_attributes= */ 0,
                        &sx);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        assert(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        အကယ်၍ (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        ပြန်ပေး 0;
}

bool dirent_is_file(const sa_nracanaa dirent *de) {
        assert(de);

        အကယ်၍ (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ပြန်ပေး false;

        အကယ်၍ (hidden_or_backup_file(de->d_name))
                ပြန်ပေး false;

        ပြန်ပေး true;
}

bool dirent_is_file_with_suffix(const sa_nracanaa dirent *de, const var_na *suffix) {
        assert(de);

        အကယ်၍ (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ပြန်ပေး false;

        အကယ်၍ (de->d_name[0] == '.')
                ပြန်ပေး false;

        အကယ်၍ (!suffix)
                ပြန်ပေး true;

        ပြန်ပေး endswith(de->d_name, suffix);
}

sa_nracanaa dirent* readdir_ensure_type(DIR *d) {
        ကိန်းပြည့် r;

        assert(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        အတွက် (;;) {
                sa_nracanaa dirent *de;

                errno = 0;
                de = readdir(d);
                အကယ်၍ (!de)
                        ပြန်ပေး NULL;

                r = dirent_ensure_type(dirfd(d), de);
                အကယ်၍ (r >= 0)
                        ပြန်ပေး de;
                အကယ်၍ (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        ပြန်ပေး NULL;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

sa_nracanaa dirent* readdir_no_dot(DIR *d) {
        assert(d);

        အတွက် (;;) {
                sa_nracanaa dirent *de;

                de = readdir_ensure_type(d);
                အကယ်၍ (!de || !dot_or_dot_dot(de->d_name))
                        ပြန်ပေး de;
        }
}
