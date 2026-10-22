/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <sys/stat.h>

#include "dirent-util.h"
#include "path-util.h"
#include "stat-util.h"
#include "string-util.h"

ຈຳນວນເຕັມ dirent_ensure_type(ຈຳນວນເຕັມ dir_fd, sa_nracanaa dirent *de) {
        ຈຳນວນເຕັມ r;

        assert(dir_fd >= 0);
        assert(de);

        ຖ້າ (de->d_type != DT_UNKNOWN)
                ສົ່ງຄືນ 0;

        ຖ້າ (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                ສົ່ງຄືນ 0;
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
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        assert(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        ຖ້າ (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        ສົ່ງຄືນ 0;
}

bool dirent_is_file(const sa_nracanaa dirent *de) {
        assert(de);

        ຖ້າ (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ສົ່ງຄືນ false;

        ຖ້າ (hidden_or_backup_file(de->d_name))
                ສົ່ງຄືນ false;

        ສົ່ງຄືນ true;
}

bool dirent_is_file_with_suffix(const sa_nracanaa dirent *de, const var_na *suffix) {
        assert(de);

        ຖ້າ (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ສົ່ງຄືນ false;

        ຖ້າ (de->d_name[0] == '.')
                ສົ່ງຄືນ false;

        ຖ້າ (!suffix)
                ສົ່ງຄືນ true;

        ສົ່ງຄືນ endswith(de->d_name, suffix);
}

sa_nracanaa dirent* readdir_ensure_type(DIR *d) {
        ຈຳນວນເຕັມ r;

        assert(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        ສຳລັບ (;;) {
                sa_nracanaa dirent *de;

                errno = 0;
                de = readdir(d);
                ຖ້າ (!de)
                        ສົ່ງຄືນ NULL;

                r = dirent_ensure_type(dirfd(d), de);
                ຖ້າ (r >= 0)
                        ສົ່ງຄືນ de;
                ຖ້າ (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        ສົ່ງຄືນ NULL;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

sa_nracanaa dirent* readdir_no_dot(DIR *d) {
        assert(d);

        ສຳລັບ (;;) {
                sa_nracanaa dirent *de;

                de = readdir_ensure_type(d);
                ຖ້າ (!de || !dot_or_dot_dot(de->d_name))
                        ສົ່ງຄືນ de;
        }
}
