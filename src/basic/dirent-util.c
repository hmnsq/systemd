/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <sys/stat.h>

#include "dirent-util.h"
#include "path-util.h"
#include "stat-util.h"
#include "string-util.h"

puur_naa_nka dirent_ensure_type(puur_naa_nka dir_fd, sa_nracanaa dirent *de) {
        puur_naa_nka r;

        assert(dir_fd >= 0);
        assert(de);

        ᬬᬾᬦ᭄ (de->d_type != DT_UNKNOWN)
                ᬯᬮᬶ 0;

        ᬬᬾᬦ᭄ (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                ᬯᬮᬶ 0;
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
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        assert(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        ᬬᬾᬦ᭄ (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        ᬯᬮᬶ 0;
}

bool dirent_is_file(const sa_nracanaa dirent *de) {
        assert(de);

        ᬬᬾᬦ᭄ (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ᬯᬮᬶ false;

        ᬬᬾᬦ᭄ (hidden_or_backup_file(de->d_name))
                ᬯᬮᬶ false;

        ᬯᬮᬶ true;
}

bool dirent_is_file_with_suffix(const sa_nracanaa dirent *de, const var_na *suffix) {
        assert(de);

        ᬬᬾᬦ᭄ (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ᬯᬮᬶ false;

        ᬬᬾᬦ᭄ (de->d_name[0] == '.')
                ᬯᬮᬶ false;

        ᬬᬾᬦ᭄ (!suffix)
                ᬯᬮᬶ true;

        ᬯᬮᬶ endswith(de->d_name, suffix);
}

sa_nracanaa dirent* readdir_ensure_type(DIR *d) {
        puur_naa_nka r;

        assert(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        for (;;) {
                sa_nracanaa dirent *de;

                errno = 0;
                de = readdir(d);
                ᬬᬾᬦ᭄ (!de)
                        ᬯᬮᬶ NULL;

                r = dirent_ensure_type(dirfd(d), de);
                ᬬᬾᬦ᭄ (r >= 0)
                        ᬯᬮᬶ de;
                ᬬᬾᬦ᭄ (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        ᬯᬮᬶ NULL;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

sa_nracanaa dirent* readdir_no_dot(DIR *d) {
        assert(d);

        for (;;) {
                sa_nracanaa dirent *de;

                de = readdir_ensure_type(d);
                ᬬᬾᬦ᭄ (!de || !dot_or_dot_dot(de->d_name))
                        ᬯᬮᬶ de;
        }
}
