/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <sys/stat.h>

#include "dirent-util.h"
#include "path-util.h"
#include "stat-util.h"
#include "string-util.h"

整数 dirent_ensure_type(整数 dir_fd, struct dirent *de) {
        整数 r;

        assert(dir_fd >= 0);
        assert(de);

        もし (de->d_type != DT_UNKNOWN)
                戻る 0;

        もし (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                戻る 0;
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
        もし (r < 0)
                戻る r;

        assert(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        もし (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        戻る 0;
}

bool dirent_is_file(const struct dirent *de) {
        assert(de);

        もし (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                戻る false;

        もし (hidden_or_backup_file(de->d_name))
                戻る false;

        戻る true;
}

bool dirent_is_file_with_suffix(const struct dirent *de, const char *suffix) {
        assert(de);

        もし (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                戻る false;

        もし (de->d_name[0] == '.')
                戻る false;

        もし (!suffix)
                戻る true;

        戻る endswith(de->d_name, suffix);
}

struct dirent* readdir_ensure_type(DIR *d) {
        整数 r;

        assert(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        ために (;;) {
                struct dirent *de;

                errno = 0;
                de = readdir(d);
                もし (!de)
                        戻る NULL;

                r = dirent_ensure_type(dirfd(d), de);
                もし (r >= 0)
                        戻る de;
                もし (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        戻る NULL;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

struct dirent* readdir_no_dot(DIR *d) {
        assert(d);

        ために (;;) {
                struct dirent *de;

                de = readdir_ensure_type(d);
                もし (!de || !dot_or_dot_dot(de->d_name))
                        戻る de;
        }
}
