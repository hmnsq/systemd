/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <sys/stat.h>

#include "dirent-util.h"
#include "path-util.h"
#include "stat-util.h"
#include "string-util.h"

පූර්ණ_සංඛ්‍යා dirent_ensure_type(පූර්ණ_සංඛ්‍යා dir_fd, sa_nracanaa dirent *de) {
        පූර්ණ_සංඛ්‍යා r;

        assert(dir_fd >= 0);
        assert(de);

        නම් (de->d_type != DT_UNKNOWN)
                ආපසු 0;

        නම් (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                ආපසු 0;
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
        නම් (r < 0)
                ආපසු r;

        assert(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        නම් (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        ආපසු 0;
}

bool dirent_is_file(const sa_nracanaa dirent *de) {
        assert(de);

        නම් (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ආපසු false;

        නම් (hidden_or_backup_file(de->d_name))
                ආපසු false;

        ආපසු true;
}

bool dirent_is_file_with_suffix(const sa_nracanaa dirent *de, const var_na *suffix) {
        assert(de);

        නම් (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ආපසු false;

        නම් (de->d_name[0] == '.')
                ආපසු false;

        නම් (!suffix)
                ආපසු true;

        ආපසු endswith(de->d_name, suffix);
}

sa_nracanaa dirent* readdir_ensure_type(DIR *d) {
        පූර්ණ_සංඛ්‍යා r;

        assert(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        සඳහා (;;) {
                sa_nracanaa dirent *de;

                errno = 0;
                de = readdir(d);
                නම් (!de)
                        ආපසු NULL;

                r = dirent_ensure_type(dirfd(d), de);
                නම් (r >= 0)
                        ආපසු de;
                නම් (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        ආපසු NULL;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

sa_nracanaa dirent* readdir_no_dot(DIR *d) {
        assert(d);

        සඳහා (;;) {
                sa_nracanaa dirent *de;

                de = readdir_ensure_type(d);
                නම් (!de || !dot_or_dot_dot(de->d_name))
                        ආපසු de;
        }
}
