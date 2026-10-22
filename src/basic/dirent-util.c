/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <sys/stat.h>

#include "dirent-util.h"
#include "path-util.h"
#include "stat-util.h"
#include "string-util.h"

цео dirent_ensure_type(цео dir_fd, struct dirent *de) {
        цео r;

        assert(dir_fd >= 0);
        assert(de);

        ако (de->d_type != DT_UNKNOWN)
                врати 0;

        ако (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                врати 0;
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
        ако (r < 0)
                врати r;

        assert(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        ако (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        врати 0;
}

bool dirent_is_file(const struct dirent *de) {
        assert(de);

        ако (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                врати false;

        ако (hidden_or_backup_file(de->d_name))
                врати false;

        врати true;
}

bool dirent_is_file_with_suffix(const struct dirent *de, const char *suffix) {
        assert(de);

        ако (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                врати false;

        ако (de->d_name[0] == '.')
                врати false;

        ако (!suffix)
                врати true;

        врати endswith(de->d_name, suffix);
}

struct dirent* readdir_ensure_type(DIR *d) {
        цео r;

        assert(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        за (;;) {
                struct dirent *de;

                errno = 0;
                de = readdir(d);
                ако (!de)
                        врати NULL;

                r = dirent_ensure_type(dirfd(d), de);
                ако (r >= 0)
                        врати de;
                ако (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        врати NULL;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

struct dirent* readdir_no_dot(DIR *d) {
        assert(d);

        за (;;) {
                struct dirent *de;

                de = readdir_ensure_type(d);
                ако (!de || !dot_or_dot_dot(de->d_name))
                        врати de;
        }
}
