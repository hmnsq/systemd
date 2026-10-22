/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <sys/stat.h>

#include "dirent-util.h"
#include "path-util.h"
#include "stat-util.h"
#include "string-util.h"

số_nguyên dirent_ensure_type(số_nguyên dir_fd, struct dirent *de) {
        số_nguyên r;

        assert(dir_fd >= 0);
        assert(de);

        nếu (de->d_type != DT_UNKNOWN)
                trả_về 0;

        nếu (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                trả_về 0;
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
        nếu (r < 0)
                trả_về r;

        assert(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        nếu (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        trả_về 0;
}

bool dirent_is_file(const struct dirent *de) {
        assert(de);

        nếu (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                trả_về false;

        nếu (hidden_or_backup_file(de->d_name))
                trả_về false;

        trả_về true;
}

bool dirent_is_file_with_suffix(const struct dirent *de, const char *suffix) {
        assert(de);

        nếu (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                trả_về false;

        nếu (de->d_name[0] == '.')
                trả_về false;

        nếu (!suffix)
                trả_về true;

        trả_về endswith(de->d_name, suffix);
}

struct dirent* readdir_ensure_type(DIR *d) {
        số_nguyên r;

        assert(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        cho (;;) {
                struct dirent *de;

                errno = 0;
                de = readdir(d);
                nếu (!de)
                        trả_về NULL;

                r = dirent_ensure_type(dirfd(d), de);
                nếu (r >= 0)
                        trả_về de;
                nếu (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        trả_về NULL;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

struct dirent* readdir_no_dot(DIR *d) {
        assert(d);

        cho (;;) {
                struct dirent *de;

                de = readdir_ensure_type(d);
                nếu (!de || !dot_or_dot_dot(de->d_name))
                        trả_về de;
        }
}
