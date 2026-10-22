/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <sys/stat.h>

#include "dirent-util.h"
#include "path-util.h"
#include "stat-util.h"
#include "string-util.h"

ціле dirent_ensure_type(ціле dir_fd, struct dirent *de) {
        ціле r;

        assert(dir_fd >= 0);
        assert(de);

        якщо (de->d_type != DT_UNKNOWN)
                повернути 0;

        якщо (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                повернути 0;
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
        якщо (r < 0)
                повернути r;

        assert(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        якщо (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        повернути 0;
}

bool dirent_is_file(const struct dirent *de) {
        assert(de);

        якщо (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                повернути false;

        якщо (hidden_or_backup_file(de->d_name))
                повернути false;

        повернути true;
}

bool dirent_is_file_with_suffix(const struct dirent *de, const char *suffix) {
        assert(de);

        якщо (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                повернути false;

        якщо (de->d_name[0] == '.')
                повернути false;

        якщо (!suffix)
                повернути true;

        повернути endswith(de->d_name, suffix);
}

struct dirent* readdir_ensure_type(DIR *d) {
        ціле r;

        assert(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        для (;;) {
                struct dirent *de;

                errno = 0;
                de = readdir(d);
                якщо (!de)
                        повернути NULL;

                r = dirent_ensure_type(dirfd(d), de);
                якщо (r >= 0)
                        повернути de;
                якщо (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        повернути NULL;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

struct dirent* readdir_no_dot(DIR *d) {
        assert(d);

        для (;;) {
                struct dirent *de;

                de = readdir_ensure_type(d);
                якщо (!de || !dot_or_dot_dot(de->d_name))
                        повернути de;
        }
}
