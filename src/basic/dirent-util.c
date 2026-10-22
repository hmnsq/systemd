/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <sys/stat.h>

#include "dirent-util.h"
#include "path-util.h"
#include "stat-util.h"
#include "string-util.h"

정수 dirent_ensure_type(정수 dir_fd, struct dirent *de) {
        정수 r;

        assert(dir_fd >= 0);
        assert(de);

        만약 (de->d_type != DT_UNKNOWN)
                반환 0;

        만약 (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                반환 0;
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
        만약 (r < 0)
                반환 r;

        assert(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        만약 (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        반환 0;
}

bool dirent_is_file(const struct dirent *de) {
        assert(de);

        만약 (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                반환 false;

        만약 (hidden_or_backup_file(de->d_name))
                반환 false;

        반환 true;
}

bool dirent_is_file_with_suffix(const struct dirent *de, const char *suffix) {
        assert(de);

        만약 (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                반환 false;

        만약 (de->d_name[0] == '.')
                반환 false;

        만약 (!suffix)
                반환 true;

        반환 endswith(de->d_name, suffix);
}

struct dirent* readdir_ensure_type(DIR *d) {
        정수 r;

        assert(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        위해 (;;) {
                struct dirent *de;

                errno = 0;
                de = readdir(d);
                만약 (!de)
                        반환 NULL;

                r = dirent_ensure_type(dirfd(d), de);
                만약 (r >= 0)
                        반환 de;
                만약 (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        반환 NULL;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

struct dirent* readdir_no_dot(DIR *d) {
        assert(d);

        위해 (;;) {
                struct dirent *de;

                de = readdir_ensure_type(d);
                만약 (!de || !dot_or_dot_dot(de->d_name))
                        반환 de;
        }
}
