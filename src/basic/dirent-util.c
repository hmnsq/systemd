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

        ᰀᰧ (de->d_type != DT_UNKNOWN)
                ᰀᰧᰌ 0;

        ᰀᰧ (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                ᰀᰧᰌ 0;
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
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        assert(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        ᰀᰧ (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        ᰀᰧᰌ 0;
}

bool dirent_is_file(const sa_nracanaa dirent *de) {
        assert(de);

        ᰀᰧ (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ᰀᰧᰌ false;

        ᰀᰧ (hidden_or_backup_file(de->d_name))
                ᰀᰧᰌ false;

        ᰀᰧᰌ true;
}

bool dirent_is_file_with_suffix(const sa_nracanaa dirent *de, const var_na *suffix) {
        assert(de);

        ᰀᰧ (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ᰀᰧᰌ false;

        ᰀᰧ (de->d_name[0] == '.')
                ᰀᰧᰌ false;

        ᰀᰧ (!suffix)
                ᰀᰧᰌ true;

        ᰀᰧᰌ endswith(de->d_name, suffix);
}

sa_nracanaa dirent* readdir_ensure_type(DIR *d) {
        puur_naa_nka r;

        assert(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        for (;;) {
                sa_nracanaa dirent *de;

                errno = 0;
                de = readdir(d);
                ᰀᰧ (!de)
                        ᰀᰧᰌ NULL;

                r = dirent_ensure_type(dirfd(d), de);
                ᰀᰧ (r >= 0)
                        ᰀᰧᰌ de;
                ᰀᰧ (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        ᰀᰧᰌ NULL;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

sa_nracanaa dirent* readdir_no_dot(DIR *d) {
        assert(d);

        for (;;) {
                sa_nracanaa dirent *de;

                de = readdir_ensure_type(d);
                ᰀᰧ (!de || !dot_or_dot_dot(de->d_name))
                        ᰀᰧᰌ de;
        }
}
