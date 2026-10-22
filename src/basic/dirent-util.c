/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <sys/stat.h>

#include "dirent-util.h"
#include "path-util.h"
#include "stat-util.h"
#include "string-util.h"

целое dirent_ensure_type(целое dir_fd, struct dirent *de) {
        целое r;

        assert(dir_fd >= 0);
        assert(de);

        если (de->d_type != DT_UNKNOWN)
                вернуть 0;

        если (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                вернуть 0;
        }

        /* Let's ask only for the type, nothing else. */
        struct statx sx;
        r = xstatx_full(dir_fd,
                        de->d_name,
                        AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ STATX_TYPE,
                        /* optional_mask= */ STATX_INO,
                        /* mandatory_attributes= */ 0,
                        &sx);
        если (r < 0)
                вернуть r;

        assert(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        если (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        вернуть 0;
}

bool dirent_is_file(const struct dirent *de) {
        assert(de);

        если (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                вернуть false;

        если (hidden_or_backup_file(de->d_name))
                вернуть false;

        вернуть true;
}

bool dirent_is_file_with_suffix(const struct dirent *de, const char *suffix) {
        assert(de);

        если (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                вернуть false;

        если (de->d_name[0] == '.')
                вернуть false;

        если (!suffix)
                вернуть true;

        вернуть endswith(de->d_name, suffix);
}

struct dirent* readdir_ensure_type(DIR *d) {
        целое r;

        assert(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        для (;;) {
                struct dirent *de;

                errno = 0;
                de = readdir(d);
                если (!de)
                        вернуть NULL;

                r = dirent_ensure_type(dirfd(d), de);
                если (r >= 0)
                        вернуть de;
                если (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        вернуть NULL;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

struct dirent* readdir_no_dot(DIR *d) {
        assert(d);

        для (;;) {
                struct dirent *de;

                de = readdir_ensure_type(d);
                если (!de || !dot_or_dot_dot(de->d_name))
                        вернуть de;
        }
}
