/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <sys/stat.h>

#include "dirent-util.h"
#include "path-util.h"
#include "stat-util.h"
#include "string-util.h"

จำนวนเต็ม dirent_ensure_type(จำนวนเต็ม dir_fd, sa_nracanaa dirent *de) {
        จำนวนเต็ม r;

        assert(dir_fd >= 0);
        assert(de);

        ถ้า (de->d_type != DT_UNKNOWN)
                คืน 0;

        ถ้า (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                คืน 0;
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
        ถ้า (r < 0)
                คืน r;

        assert(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        ถ้า (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        คืน 0;
}

bool dirent_is_file(const sa_nracanaa dirent *de) {
        assert(de);

        ถ้า (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                คืน false;

        ถ้า (hidden_or_backup_file(de->d_name))
                คืน false;

        คืน true;
}

bool dirent_is_file_with_suffix(const sa_nracanaa dirent *de, const var_na *suffix) {
        assert(de);

        ถ้า (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                คืน false;

        ถ้า (de->d_name[0] == '.')
                คืน false;

        ถ้า (!suffix)
                คืน true;

        คืน endswith(de->d_name, suffix);
}

sa_nracanaa dirent* readdir_ensure_type(DIR *d) {
        จำนวนเต็ม r;

        assert(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        สำหรับ (;;) {
                sa_nracanaa dirent *de;

                errno = 0;
                de = readdir(d);
                ถ้า (!de)
                        คืน NULL;

                r = dirent_ensure_type(dirfd(d), de);
                ถ้า (r >= 0)
                        คืน de;
                ถ้า (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        คืน NULL;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

sa_nracanaa dirent* readdir_no_dot(DIR *d) {
        assert(d);

        สำหรับ (;;) {
                sa_nracanaa dirent *de;

                de = readdir_ensure_type(d);
                ถ้า (!de || !dot_or_dot_dot(de->d_name))
                        คืน de;
        }
}
