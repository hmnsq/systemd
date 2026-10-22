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

        ᮚᮦᮔ᮪ (de->d_type != DT_UNKNOWN)
                ᮝᮜᮤ 0;

        ᮚᮦᮔ᮪ (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                ᮝᮜᮤ 0;
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
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        assert(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        ᮚᮦᮔ᮪ (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        ᮝᮜᮤ 0;
}

bool dirent_is_file(const sa_nracanaa dirent *de) {
        assert(de);

        ᮚᮦᮔ᮪ (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (hidden_or_backup_file(de->d_name))
                ᮝᮜᮤ false;

        ᮝᮜᮤ true;
}

bool dirent_is_file_with_suffix(const sa_nracanaa dirent *de, const var_na *suffix) {
        assert(de);

        ᮚᮦᮔ᮪ (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (de->d_name[0] == '.')
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (!suffix)
                ᮝᮜᮤ true;

        ᮝᮜᮤ endswith(de->d_name, suffix);
}

sa_nracanaa dirent* readdir_ensure_type(DIR *d) {
        puur_naa_nka r;

        assert(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        for (;;) {
                sa_nracanaa dirent *de;

                errno = 0;
                de = readdir(d);
                ᮚᮦᮔ᮪ (!de)
                        ᮝᮜᮤ NULL;

                r = dirent_ensure_type(dirfd(d), de);
                ᮚᮦᮔ᮪ (r >= 0)
                        ᮝᮜᮤ de;
                ᮚᮦᮔ᮪ (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        ᮝᮜᮤ NULL;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

sa_nracanaa dirent* readdir_no_dot(DIR *d) {
        assert(d);

        for (;;) {
                sa_nracanaa dirent *de;

                de = readdir_ensure_type(d);
                ᮚᮦᮔ᮪ (!de || !dot_or_dot_dot(de->d_name))
                        ᮝᮜᮤ de;
        }
}
